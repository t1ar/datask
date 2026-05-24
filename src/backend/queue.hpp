#pragma once
#include "task.hpp"

//Node for the FIFO Queue 
//managing chronological focus tasks.
struct QueueNode {
    Task* taskPtr;
    QueueNode* next = nullptr;
};

//Structure maintaining references 
//to both the front and rear of the queue.
struct TaskQueue {
    QueueNode* front = nullptr;
    QueueNode* rear = nullptr;
};

//Adds a newly created or loaded task to the back of the queue.
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

//Removes and retrieves the oldest task 
//sitting at the front of the queue.
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