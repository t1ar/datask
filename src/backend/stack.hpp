#pragma once
#include "task.hpp"

//Node for the LIFO Stack managing the Ctrl+Z Undo history.
struct StackNode {
    Task* taskPtr;
    StackNode* next;
};

//Pushes a recently deleted task onto the top of the Undo Stack.
inline void push(StackNode*& top, Task* deletedTask){
    StackNode* newNode = new StackNode;
    newNode->taskPtr = deletedTask;
    newNode->next = top;
    top = newNode;
};

//Pops the most recently deleted task off 
//the stack for restoration.
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