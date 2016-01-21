//
// Created by mereckaj on 15/01/16.
//

#include "BST.hpp"

Node::Node(int k) {
    key = k;
    left = NULL;
    right = NULL;
}

BST::BST() {
    tatas_lock.init();
}

Node* BST::removeNodeLocked(int key){
    tatas_lock.acquirePessimistic();
    Node* res = remove(key);
    tatas_lock.release();
    return res;
}

int BST::addNodeLocked(Node* node){
    tatas_lock.acquirePessimistic();
    int res = add(node);
    tatas_lock.release();
    return res;
}

int BST::add(Node *new_node) {
    Node *volatile*pp = &root;
    Node *p = root;
    while (p) {
        if (new_node->key < p->key) {
            pp = &p->left;
        }
        else if (new_node->key > p->key) {
            pp = &p->right;
        }
        else {
            return 0;
        }
        p = *pp;
    }
    *pp = new_node;
    return 1;
}

Node *BST::remove(INT64 key) {
    Node * volatile *pp = &root;
    Node *p = root;
    while (p) {
        if (key < p->key) {
            pp = &p->left;
        } else if (key > p->key) {
            pp = &p->right;
        } else {
            break;
        }
        p = *pp;
    }
    if (p == NULL)
        return NULL;
    if (p->left == NULL && p->right == NULL) {
        *pp = NULL;
    } else if (p->left == NULL) {
        *pp = p->right;
    } else if (p->right == NULL) {
        *pp = p->left;
    } else {
        Node *r = p->right;
        Node **ppr = &p->right;
        while (r!=NULL && r->left) {
            ppr = &r->left;
            r = r->left;
        }
        if(r!=NULL) {
            p->key = r->key;
            p = r;
            *ppr = r->right;
        }
    }
    return p;
}
