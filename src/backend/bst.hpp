#pragma once
#include "task.hpp"

struct BSTNode {
    Task* taskPtr;
    BSTNode* left = nullptr;
    BSTNode* right = nullptr;
};

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