//
// Created by mereckaj on 15/01/16.
//

#include "BST.hpp"

#define LOCK 0
#define TRANSACTION 1
#define MAXATTEMPT 16

Node::Node(int k) {
    key = k;
    left = NULL;
    right = NULL;
}

BST::BST() {
    tatas_lock.init();
    root = NULL;
}

Node *BST::removeTATAS(int key) {
    tatas_lock.acquirePessimistic();
    Node *res = remove(key);
    tatas_lock.release();
    return res;
}

int BST::addTATAS(Node *node) {
    tatas_lock.acquirePessimistic();
    int res = add(node);
    tatas_lock.release();
    return res;
}

int BST::add(Node *new_node) {
    Node *volatile *pp = &root;
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
    Node *volatile *pp = &root;
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
        while (r != NULL && r->left) {
            ppr = &r->left;
            r = r->left;
        }
        if (r != NULL) {
            p->key = r->key;
            p = r;
            *ppr = r->right;
        }
    }
    return p;
}

int BST::addHLE(Node *node) {
    int res;
    while (1) {
        int status = _xbegin();
        if (status == _XBEGIN_STARTED) {
            res = add(node);
            _xend();
            break;
        } else {
            res = addTATAS(node);
        }
    }
    return res;
}

int BST::addRTM(Node *node) {
    int state = TRANSACTION;
    int attempt = 1;
    int nabort = 0;
    volatile UINT64 wait = 0;

    while (1) {
        UINT status = _XBEGIN_STARTED;
        if (state == TRANSACTION) {
            status = _xbegin();
        }
        else {
            tatas_lock.acquireOptimistic();
        }

        if (status == _XBEGIN_STARTED) {
            if (status == TRANSACTION && tatas_lock.getValue()) {
                _xabort(0xA0);
                nabort++;
            }

            add(node);

            if (state == TRANSACTION) {
                _xend();
            }
            else {
                tatas_lock.release();
            }
            break;
        }
        else {
            if (tatas_lock.getValue()) {
                do {
                    _mm_pause();
                } while (tatas_lock.getValue());
            }
            else {
                volatile UINT64 wait = attempt;
                while (wait--);
            }
            if (++attempt >= MAXATTEMPT) {
                state = LOCK;
            }
        }
    }
}

Node *BST::removeHLE(int key) {
    Node *res;
    while (1) {
        int status = _xbegin();
        if (status == _XBEGIN_STARTED) {
            res = remove(key);
            _xend();
            break;
        } else {
            res = removeTATAS(key);
        }
    }
    return res;
}

Node *BST::removeRTM(int key) {
    return nullptr;
}
