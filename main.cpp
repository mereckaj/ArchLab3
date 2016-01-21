#include <iostream>
#include <random>
#include <queue>
#include <tuple>
#include "helper.h"
#include "BST.hpp"
#include "Misc.hpp"

#define NOPS 1000
#define NSECONDS 1
#define PREALLOCATED_NODE_COUNT 100000
/*
 * 0 = TATAS
 * 1 = HLE
 * 2 = RTM
 */
#define LOCK_TYPE 2


std::tuple<int, int> range_1(0, 15);
std::tuple<int, int> range_2(0, 256);
std::tuple<int, int> range_3(0, 4096);
std::tuple<int, int> range_4(0, 65535);
std::tuple<int, int> range_5(0, 1048576);

std::vector<std::tuple<int, int>> ranges = {range_1, range_2, range_3, range_4, range_5};
static int currentRange = 0;

BST *bst;
Result *resultArray;

void PrintDebug(int threadID, std::string message) {
    std::cout << std::to_string(threadID) << "\t" << message << std::endl;
}


BST *preffilTree() {
    BST *new_bst = new BST();
    int range_min = std::get<0>(ranges[currentRange]);
    int range_max = std::get<1>(ranges[currentRange]);

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist((unsigned long) range_min, (unsigned long) range_max);

    int numberOfNodesToAdd = (range_max - range_min) / 2;

    for (int i = 0; i < numberOfNodesToAdd; i++) {
        int key = (int) dist(rng);

        Node *newNode = new Node(key);
        new_bst->add(newNode);
    }

    return new_bst;
}

void populateNodeQueue(std::queue<Node *> *node_queue, Result *result) {
    for (int i = 0; i < PREALLOCATED_NODE_COUNT; i++) {
        (*node_queue).push(new Node(0));
    }
    (*result).nodesCreated = (*node_queue).size();
}

WORKER worker(void *vthread) {
    Result result;
    std::queue<Node *> nodeQueue;
    std::mt19937 rng;
    UINT64 nops_done = 0;
    UINT64 adds = 0;
    UINT64 removes = 0;
    UINT64 malloced = 0;
    UINT64 readded = 0;
    UINT64 failedAdds = 0;
    UINT64 failedRems = 0;

    int thread_id = (int) ((size_t) vthread);
    result.threadID = (UINT64) thread_id;
    int range_min = std::get<0>(ranges[currentRange]);
    int range_max = std::get<1>(ranges[currentRange]);

    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist((unsigned long) range_min, (unsigned long) range_max);

    UINT64 tstart = getWallClockMS();
    UINT64 tend;

    populateNodeQueue(&nodeQueue, &result);


    while (true) {
        for (int i = 0; i < NOPS; i++) {

            int add_or_rem = (int) dist(rng);
            if (add_or_rem % 2 == 0) {
                adds++;
                int key = (int) dist(rng);
                Node *newNode;
                //Check if there are nodes in the free node queue
                if (!nodeQueue.empty()) {
                    //There are nodes so take an old one
                    newNode = nodeQueue.front();
                    nodeQueue.pop();
                    newNode->key = key;
                } else {
                    // THere are no nodes so create a new one
                    malloced++;
                    newNode = new Node(key);
                }
                //Add this node to the tree
#if LOCK_TYPE == 0
                int res = (*bst).addTATAS(newNode);
#elif LOCK_TYPE == 1
                int res = (*bst).addHLE(newNode);
#elif LOCK_TYPE == 2
                int res = (*bst).addRTM(newNode);
#endif
                if (res == 0) {
                    failedAdds++;
                }

            } else {
                removes++;
                int key = (int) dist(rng);

#if LOCK_TYPE == 0
                Node *oldNode = (*bst).removeTATAS(key);
#elif LOCK_TYPE == 1
                Node *oldNode = (*bst).removeHLE(key);
#elif LOCK_TYPE == 2
                Node *oldNode = (*bst).removeRTM(key);
#endif

                // Add node back to free node queue
                if (oldNode != NULL) {
                    oldNode->key = 0;
                    oldNode->left = NULL;
                    oldNode->right = NULL;
                    nodeQueue.push(oldNode);
                    readded++;
                } else {
                    failedRems++;
                }
            }
        }
        nops_done += NOPS;
        tend = getWallClockMS();
        if ((tend - tstart) > NSECONDS * 1000) {
            break;
        }
    }
    result.nodesLeftInQueue = nodeQueue.size();
    result.nops = nops_done;
    result.adds = adds;
    result.removes = removes;
    result.mallocedNodes = malloced;
    result.runTime = tend - tstart;
    result.nodesReaddedToQueue = readded;
    result.failedAdds = failedAdds;
    result.failedRemoves = failedRems;
    resultArray[thread_id] = result;
}

void parseResults(Result *pResult, int size, int nt, int rng) {
    using namespace std;

    Result average;
    average.threadID = (UINT64) nt;

    for (int i = 0; i < size; i++) {
        average.nops += pResult[i].nops;
        average.nodesCreated += pResult[i].nodesCreated;
        average.nodesLeftInQueue += pResult[i].nodesLeftInQueue;
        average.nodesReaddedToQueue += pResult[i].nodesReaddedToQueue;
        average.mallocedNodes += pResult[i].mallocedNodes;
        average.adds += pResult[i].adds;
        average.removes += pResult[i].removes;
        average.failedAdds += pResult[i].failedAdds;
        average.failedRemoves += pResult[i].failedRemoves;
    }

    average.nops = average.nops / size;
    average.nodesCreated = average.nodesCreated / size;
    average.nodesLeftInQueue = average.nodesLeftInQueue / size;
    average.nodesReaddedToQueue = average.nodesReaddedToQueue / size;
    average.mallocedNodes = average.mallocedNodes / size;
    average.adds = average.adds / size;
    average.removes = average.removes / size;
    average.failedAdds = average.failedAdds / size;
    average.failedRemoves = average.failedRemoves / size;

    cout << to_string(nt) << "\t";
    cout << to_string(average.nops) << "\t";
    cout << to_string(average.nodesCreated) << "\t";
    cout << to_string(average.nodesLeftInQueue) << "\t";
    cout << to_string(average.nodesReaddedToQueue) << "\t";
    cout << to_string(average.mallocedNodes) << "\t";
    cout << to_string(average.adds) << "\t";
    cout << to_string(average.failedAdds) << "\t";
    cout << to_string(average.removes) << "\t";
    cout << to_string(average.failedRemoves) << "\t";
    cout << "(" << to_string(std::get<0>(ranges[rng])) << "," << to_string(std::get<1>(ranges[rng])) <<
    ")" << endl;
}

int main(int argc, char **argv) {


    std::cout << "NT:\t" << "NOPS:\t" << "NCREATED:\t" << "NLEFT:\t" << "READDED:\t" << "MALLOCS:\t" << "ADD:\t" <<
    "FADD:\t" << "REM:\t" << "FREM:\t" << "RANGE:\t" << std::endl;

    int NCPU = getNumberOfCPUs() * 2;

    for (size_t range = 0; range < ranges.size(); range++) {
        currentRange = (int) range;
        for (int threadCount = 1; threadCount <= NCPU; threadCount++) {
            THREADH threads[threadCount];

            //Create a new tree
            bst = preffilTree();
            resultArray = new Result[threadCount];

            for (int thread = 0; thread < threadCount; thread++) {
                createThread(&threads[thread], worker, (void *) thread);
            }
            waitForThreadsToFinish((unsigned int) threadCount, threads);
            parseResults(resultArray, threadCount, threadCount, currentRange);
        }
    }
}