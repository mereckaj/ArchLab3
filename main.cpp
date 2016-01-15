#include <iostream>
#include <random>
#include <queue>
#include "helper.h"
#include "BST.hpp"

#define NOPS 10000
#define NSECONDS 1
#define PREALLOCATED_NODE_COUNT 10000

std::tuple<int, int> range_1(0, 15);
std::tuple<int, int> range_2(0, 256);
std::tuple<int, int> range_3(0, 4096);
std::tuple<int, int> range_4(0, 65535);
std::tuple<int, int> range_5(0, 1048576);

std::vector<std::tuple<int, int>> ranges = {range_1, range_2 , range_3, range_4, range_5};
static int current_range = 0;

BST *bst;

BST *prefil_BST() {
    BST *new_bst = new BST();
    int range_min = std::get<0>(ranges[current_range]);
    int range_max = std::get<1>(ranges[current_range]);
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist((unsigned long) range_min, (unsigned long) range_max);

    int number_of_nodes_to_add = (range_max - range_min) / 2; // Integer division
    std::cout << "Adding " << std::to_string(number_of_nodes_to_add) << " to tree" << std::endl;
    for (int i = 0; i < number_of_nodes_to_add; i++) {
        int key = (int) dist(rng);
        Node *new_node = new Node(key);
        new_bst->add(new_node);
    }
    std::cout << "Added " << std::to_string(number_of_nodes_to_add) << " to tree" << std::endl;
    return new_bst;
}

WORKER worker(void *vthread) {
    int thread_id = (int) ((size_t) vthread);
    int range_min = std::get<0>(ranges[current_range]);
    int range_max = std::get<1>(ranges[current_range]);
    int nops_done = 0;
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist((unsigned long) range_min, (unsigned long) range_max);

    UINT64 tstart = getWallClockMS();
    //Create a queue of pre-allocated nodes;
    std::queue<Node *> node_queue;

    std::cout << "Populating pre allocated node queue" << std::endl;

    for (int i = 0; i < PREALLOCATED_NODE_COUNT; i++) {
        node_queue.push(new Node(0));
    }

    std::cout << "Pre allocated node queue size: " << node_queue.size() << std::endl;

    while (true) {
        for (int i = 0; i < NOPS; i++) {

            int add_or_rem = (int) dist(rng);
            if (add_or_rem % 2 == 0) {
                int key = (int) dist(rng);
                Node *new_node;
                //Check if there are nodes in the free node queue
                if (!node_queue.empty()) {
                    //There are nodes so take an old one
                    new_node = node_queue.front();
                    node_queue.pop();
                    new_node->key = key;
                } else {
                    // THere are no nodes so create a new one
                    new_node = new Node(key);
                }
                //Add this node to the tree
                bst->add(new_node);

            } else {
                int key = (int) dist(rng);
                Node *old_node = bst->remove(key);
                // Add node back to free node queue
                if (old_node != NULL) {
                    old_node->key = 0;
                    old_node->left = NULL;
                    old_node->right = NULL;

                    node_queue.push(old_node);
//                    std::cout << std::to_string(node_queue.size()) << " nodes remaining" << std::endl;
                }
            }
            nops_done += NOPS;
            if ((getWallClockMS() - tstart) > NSECONDS * 1000) {
                break;
            }
        }


        return EXIT_SUCCESS;
    }
}

int main(int argc, char **argv) {
    int NCPU = getNumberOfCPUs() * 2;

    for (size_t range = 0; range < ranges.size(); range++) {
        current_range = (int) range;
        for (int thread_count = 0; thread_count <= NCPU; thread_count++) {
            THREADH threads[thread_count];

            //Create a new tree
            bst = prefil_BST();

            for (int thread = 0; thread < thread_count; thread++) {
                createThread(&threads[thread], worker, (void *) thread);
            }
            waitForThreadsToFinish((unsigned int) thread_count, threads);
        }
    }
}