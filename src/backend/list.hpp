#pragma once
#include "task.hpp"
#include <iostream>

//Inserts a newly created task 
//at the tail of the doubly linked list.
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

//Split the thing into 2
//Helper function for Merge Sort. 
//Splits a doubly linked list into two halves.
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

// Merges two sorted doubly linked lists back together. 
// boolean for sorting purpose
inline ListNode* mergeLists(ListNode* a, ListNode* b, bool sortByPriority){
    if (!a) return b;
    if (!b) return a;

    ListNode dummy;
    ListNode* current = &dummy;

    while (a != nullptr && b != nullptr){
        bool aComesFirst = false;
        
        // Sort by Priority OR Deadline
        if (sortByPriority) {
            aComesFirst = (a->taskPtr->priority <= b->taskPtr->priority);
        } else {
            aComesFirst = (a->taskPtr->deadline <= b->taskPtr->deadline);
        }

        if (aComesFirst){
            current->next = a; a->prev = current; a = a->next;
        } else {
            current->next = b; b->prev = current; b = b->next;
        }
        current = current->next;
    }

    if (a != nullptr){ current->next = a; a->prev = current; } 
    else { current->next = b; b->prev = current; }

    ListNode* realHead = dummy.next;
    if (realHead) realHead->prev = nullptr;
    return realHead;
};

// Executes an O(N log N) recursive Merge Sort on the doubly linked list.
inline ListNode* mergeSort(ListNode* head, bool sortByPriority){
    if (!head || head->next == nullptr) return head;
    
    ListNode* secondHalf = splitList(head);
    head = mergeSort(head, sortByPriority);
    secondHalf = mergeSort(secondHalf, sortByPriority);

    return mergeLists(head, secondHalf, sortByPriority);
};