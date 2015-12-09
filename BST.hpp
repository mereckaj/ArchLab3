//
// Created by mereckaj on 12/7/15.
//

#ifndef ARCHLAB3_BINARYSEARCHTREE_HPP
#define ARCHLAB3_BINARYSEARCHTREE_HPP

#include "TestAndTestAndSetLock.hpp"
#include "HardwareLockElision.hpp"
#include "helper.h"
struct tree_node
{
    tree_node* left;
    tree_node* right;
    UINT64 data;
};

class BinarySearchTree
{
private:
    TestAndTestAndSetLock ttas_lock;
    HardwareLockElision hle_lock;
    UINT64 elementCount;
public:
    void insertWithTestAndTestAndSetLock(UINT64);
    void insertWithHLE(UINT64);
    void insertWithRTM(UINT64);
    void removeWithTestAndTestAndSetLock(UINT64);
    void removeWithHLE(UINT64);
    void removeWithRTM(UINT64);
    BinarySearchTree();
    bool isEmpty();
    UINT64 getSize();
    void remove(UINT64);
    void insert(UINT64);
    BinarySearchTree(UINT64 root);
    tree_node* root;
};

#endif //ARCHLAB3_BINARYSEARCHTREE_HPP
