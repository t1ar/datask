#pragma once
#include <string>

struct Task {
    std::string id;
    std::string title;
    int priority;
    int deadline;
    bool completed;
};

//doubly link, need it to go both ways
struct ListNode {
    Task* taskPtr;
    ListNode* next = nullptr;
    ListNode* prev = nullptr;
};