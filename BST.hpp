//
// Created by mereckaj on 15/01/16.
//

#ifndef ARCHLAB3_BST_HPP
#define ARCHLAB3_BST_HPP

#include "helper.h"

class Node{

public:
    INT64 volatile key;
    Node* left;
    Node* right;

    Node(int key);
};

class BST{
public:
    BST();
    int add(Node* new_node);
    Node* remove(INT64 key);

private:
    Node* volatile root;
};
#endif //ARCHLAB3_BST_HPP
