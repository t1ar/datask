#pragma once
#include "task.hpp"

struct StackNode {
    Task* taskPtr;
    StackNode* next;
};

inline void push(StackNode*& top, Task* deletedTask){
    StackNode* newNode = new StackNode;
    newNode->taskPtr = deletedTask;
    newNode->next = top;
    top = newNode;
};

inline Task* pop(StackNode*& top){
    if (!top) {
        return nullptr;
    }
    StackNode* nodeToDelete = top;
    Task* savedTask = nodeToDelete->taskPtr;
    top = top->next;
    delete nodeToDelete;
    return savedTask; 
};