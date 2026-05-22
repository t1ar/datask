#pragma once
#include "task.hpp"
#include <iostream>

inline void insertTask(ListNode*& head, Task* newTask) {
    ListNode* newNode = new ListNode{newTask, nullptr, nullptr};
    if (!head) {
        head = newNode;
        return;
    }
    ListNode* temp = head;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = newNode;
    newNode->prev = temp;
};

//split the thing into 2
inline ListNode* splitList(ListNode* head) {
    ListNode* fast = head;
    ListNode* slow = head;
    
    while (fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    
    ListNode* temp = slow->next;
    slow->next = nullptr;

    if (temp != nullptr){
        temp->prev = nullptr;
    }
    
    return temp;
};

inline ListNode* mergeLists(ListNode* a, ListNode* b){
    if (!a) return b;
    if (!b) return a;

    ListNode dummy;
    ListNode* current = &dummy;

    while (a != nullptr && b != nullptr){
        if (a->taskPtr->deadline <= b->taskPtr->deadline){
            current->next = a;
            a->prev = current;
            a = a->next;
        } else {
            current->next = b;
            b->prev = current;
            b = b->next;
        }
        current = current->next;
    }

    if (a != nullptr){
        current->next = a;
        a->prev = current;
    } else {
        current->next = b;
        b->prev = current;
    }

    ListNode* realHead = dummy.next;
    if (realHead) realHead->prev = nullptr;
    
    return realHead;

};

inline ListNode* mergeSort(ListNode* head){
    if (!head || head->next == nullptr){
        return head;
    }
    ListNode* secondHalf = splitList(head);
    head = mergeSort(head);
    secondHalf = mergeSort(secondHalf);

    return mergeLists(head,secondHalf);
};