#pragma once
#include "task.hpp"

//Node for the Binary Search Tree, 
//sorted strictly by integer Priority.
struct BSTNode {
    Task* taskPtr;
    BSTNode* left = nullptr;
    BSTNode* right = nullptr;
};

//Recursively inserts a task into the 
//BST based on its priority level.
inline BSTNode* insertPriority(BSTNode* root, Task* newTask){
    if (!root){
        return new BSTNode{newTask,nullptr,nullptr};
    }

    if (newTask->priority < root->taskPtr->priority){
        root->left = insertPriority(root->left,newTask);
    } else {
        root->right = insertPriority(root->right,newTask);
    }

    return root;
};

//Traverses down the far-left branch of the BST 
//to find the absolute highest priority task.
inline Task* pullHighestPriority(BSTNode* root){
    if (!root){
        return nullptr;
    }
    BSTNode* current = root;
    while (current->left){
        current = current->left;
    }
    return current->taskPtr;
};