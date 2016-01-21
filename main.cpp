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
                if (bst->addNodeLocked(newNode) == 0) {
                    failedAdds++;
                }

            } else {
                removes++;
                int key = (int) dist(rng);
                Node *oldNode = bst->removeNodeLocked(key);

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
    for (int i = 0; i < size; i++) {
        cout << "NT:\t" << to_string(nt) << "\t";
        cout << "TID:\t" << to_string(pResult[i].threadID) << "\t";
        cout << "NOPS:\t" << to_string(pResult[i].nops) << "\t";
        cout << "RUN TIME:\t" << to_string((((double) pResult[i].runTime) / 1000)) << "\t";
        cout << "NCREATED:\t" << to_string(pResult[i].nodesCreated) << "\t";
        cout << "NLEFT:\t" << to_string(pResult[i].nodesLeftInQueue) << "\t";
        cout << "READDED:\t" << to_string(pResult[i].nodesReaddedToQueue) << "\t";
        cout << "MALLOCS:\t" << to_string(pResult[i].mallocedNodes) << "\t";
        cout << "ADD:\t" << to_string(pResult[i].adds) << "\t";
        cout << "FADD:\t" << to_string(pResult[i].failedAdds) << "\t";
        cout << "REM:\t" << to_string(pResult[i].removes) << "\t";
        cout << "FREM:\t" << to_string(pResult[i].failedRemoves) << "\t";
        cout << "RANGE:\t (" << to_string(std::get<0>(ranges[rng])) << "," << to_string(std::get<1>(ranges[rng])) <<
        ")" << endl;
    }
    cout << endl;
}

int main(int argc, char **argv) {
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