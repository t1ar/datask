#pragma once
#include "task.hpp"
#include "list.hpp"
#include "graph.hpp"
#include "bst.hpp"

//Recursively deletes all nodes 
//in the Binary Search Tree to free heap memory.
inline void freeBST(BSTNode* root){
    if (!root) {
        return;
    }
    freeBST(root->left);
    freeBST(root->right);

    delete root;
};

//Destroys a single GraphNode and iterates 
//through its linked list of outgoing edges to free them.
inline void freeGraph(GraphNode* graphHead){
    DependencyEdge* currEdge = graphHead->outgoingEdge;
    while (currEdge){
        DependencyEdge* nextEdge = currEdge->next;
        delete currEdge;
        currEdge = nextEdge;
    }
    delete graphHead;
};

//Iterates linearly through the Doubly Linked List 
//to destroy every task and node.
inline void freeList(ListNode* head){
    ListNode* curr = head;
    while (curr){
        ListNode* nextNode = curr->next;
        delete curr->taskPtr;
        delete curr;
        curr = nextNode;
    }
};