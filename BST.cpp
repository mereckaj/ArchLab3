/*
 * Code adapted from: http://www.cplusplus.com/forum/general/1551/
 */
#include <iostream>
#include "BST.hpp"

using namespace std;

void BinarySearchTree::insert(UINT64 d) {
    tree_node *t = new tree_node;
    tree_node *parent;
    t->data = d;
    t->left = NULL;
    t->right = NULL;
    parent = NULL;

    // is this a new tree?
    if (isEmpty()) {
        root = t;
    } else {
        //Note: ALL insertions are as leaf nodes
        tree_node *curr;
        curr = root;
        // Find the Node's parent
        while (curr) {
            parent = curr;
            if (t->data == curr->data) {
                // this data is already there
                return;
            } else if (t->data > curr->data) {
                curr = curr->right;
            } else {
                curr = curr->left;
            }
        }
        if (t->data < parent->data) {
            parent->left = t;
        } else {
            parent->right = t;
        }
    }
    elementCount++;
}

void BinarySearchTree::remove(UINT64 d) {
    //Locate the element
    bool found = false;
    if (isEmpty()) {
        cout << " This Tree is empty! " << endl;
        return;
    }

    tree_node *curr;
    tree_node *parent;
    curr = root;

    while (curr != NULL) {
        if (curr->data == d) {
            found = true;
            break;
        }
        else {
            parent = curr;
            if (d > curr->data) curr = curr->right;
            else curr = curr->left;
        }
    }
    if (!found) {
        cout << " Data not found! " << endl;
        return;
    }


    // 3 cases :
    // 1. We're removing a leaf node
    // 2. We're removing a node with a single child
    // 3. we're removing a node with 2 children

    // Node with single child
    if ((curr->left == NULL && curr->right != NULL) || (curr->left != NULL
                                                        && curr->right == NULL)) {
        if (curr->left == NULL && curr->right != NULL) {
            if (parent->left == curr) {
                parent->left = curr->right;
                delete curr;
            }
            else {
                parent->right = curr->right;
                delete curr;
            }
        }
        else // left child present, no right child
        {
            if (parent->left == curr) {
                parent->left = curr->left;
                delete curr;
            }
            else {
                parent->right = curr->left;
                delete curr;
            }
        }
        return;
    }

    //We're looking at a leaf node
    if (curr->left == NULL && curr->right == NULL) {
        if (parent->left == curr) parent->left = NULL;
        else parent->right = NULL;
        delete curr;
        return;
    }


    //Node with 2 children
    // replace node with smallest value in right subtree
    if (curr->left != NULL && curr->right != NULL) {
        tree_node *chkr;
        chkr = curr->right;
        if ((chkr->left == NULL) && (chkr->right == NULL)) {
            curr = chkr;
            delete chkr;
            curr->right = NULL;
        }
        else // right child has children
        {
            //if the node's right child has a left child
            // Move all the way down left to locate smallest element

            if ((curr->right)->left != NULL) {
                tree_node *lcurr;
                tree_node *lcurrp;
                lcurrp = curr->right;
                lcurr = (curr->right)->left;
                while (lcurr->left != NULL) {
                    lcurrp = lcurr;
                    lcurr = lcurr->left;
                }
                curr->data = lcurr->data;
                delete lcurr;
                lcurrp->left = NULL;
            }
            else {
                tree_node *tmp;
                tmp = curr->right;
                curr->data = tmp->data;
                curr->right = tmp->right;
                delete tmp;
            }

        }
        return;
    }

}

bool BinarySearchTree::isEmpty() {
    return root == NULL && elementCount == 0;
}

BinarySearchTree::BinarySearchTree() {
    root = NULL;
    elementCount = 0;
    ttas_lock.init();
    hle_lock.init();
}

void BinarySearchTree::insertWithTestAndTestAndSetLock(UINT64 i) {
    ttas_lock.acquirePessimistic();
    insert(i);
    ttas_lock.release();
}

void BinarySearchTree::insertWithHLE(UINT64 i) {
    hle_lock.acquire();
    insert(i);
    hle_lock.release();
}

void BinarySearchTree::insertWithRTM(UINT64 i) {

}

UINT64 BinarySearchTree::getSize() {
    return elementCount;
}

BinarySearchTree::BinarySearchTree(UINT64 root) {
    BinarySearchTree();
    insert(root);
}

void BinarySearchTree::removeWithTestAndTestAndSetLock(unsigned long long int i) {
    ttas_lock.acquirePessimistic();
    remove(i);
    ttas_lock.release();
}

void BinarySearchTree::removeWithHLE(unsigned long long int i) {

}

void BinarySearchTree::removeWithRTM(unsigned long long int i) {

}
