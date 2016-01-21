//
// Created by mereckaj on 15/01/16.
//

#ifndef ARCHLAB3_BST_HPP
#define ARCHLAB3_BST_HPP

#include "helper.h"
#include "TestAndTestAndSetLock.hpp"

class Node {

public:
    INT64 volatile key;
    Node *left;
    Node *right;

    Node(int key);
};

class BST {
public:
    BST();

    int add(Node *new_node);

    Node *remove(INT64 key);

    int addTATAS(Node *node);

    int addHLE(Node *node);

    int addRTM(Node *node);

    Node *removeTATAS(int key);

    Node *removeHLE(int key);

    Node *removeRTM(int key);

private:
    Node *volatile root;

    TestAndTestAndSetLock tatas_lock;

};

#endif //ARCHLAB3_BST_HPP
