#pragma once
#include "task.hpp"

struct QueueNode {
    Task* taskPtr;
    QueueNode* next = nullptr;
};

struct TaskQueue {
    QueueNode* front = nullptr;
    QueueNode* rear = nullptr;
};

inline void enqueue(TaskQueue& q, Task* newTask){
    QueueNode* newNode = new QueueNode{newTask, nullptr};
    if (!q.rear) {
        q.front = newNode;
        q.rear = newNode;
        return;
    }
    q.rear->next = newNode;
    q.rear = newNode;
};

inline Task* dequeue(TaskQueue& q){
    if (!q.front) {
        return nullptr;
    }
    QueueNode* toDelete = q.front;
    Task* savedTask = toDelete->taskPtr;
    q.front = q.front->next;
    if (!q.front){
        q.rear = nullptr;
    }
    delete toDelete;
    return savedTask;
};