#pragma once
#include "list.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//Saves the Doubly Linked List from RAM into a CSV file on disk.
inline void saveTasks(ListNode* head, std::string filename){
    std::ofstream outFile(filename);
    if (!outFile){
        std::cout << "File failed to opened!\n";
        return;
    }
    ListNode* curr = head;
    while (curr) {
        outFile << curr->taskPtr->id << ","
                << curr->taskPtr->title << ","
                << curr->taskPtr->priority << ","
                << curr->taskPtr->deadline << ","
                << curr->taskPtr->completed << ","
                << curr->taskPtr->rawDependencies << "\n";
        curr = curr->next;
    }
    outFile.close();
    std::cout << "Task is successfully saved to " << filename << "!\n";
};

//Loads a CSV file from disk into a fresh Doubly Linked List in RAM
inline ListNode* loadTasks(std::string filename){
    ListNode* head = nullptr;
    std::ifstream inFile(filename);
    if (!inFile){
        std::cout << "File failed to opened!\n";
        return head;
    }
    std::string line;
    while (std::getline(inFile,line)){
        std::stringstream ss(line);
        std::string id,title,priorityStr,deadlineStr,completedStr;

        std::getline(ss,id,',');
        std::getline(ss,title,',');
        std::getline(ss,priorityStr,',');
        std::getline(ss,deadlineStr,',');
        std::getline(ss,completedStr,',');

        std::string rawDeps = "";
        std::getline(ss,rawDeps);

        int priority = std::stoi(priorityStr);
        int deadline = std::stoi(deadlineStr);
        bool completed = (completedStr == "1");

        Task* loadedTask = new Task {id,title,priority,deadline,completed,rawDeps};
        insertTask(head,loadedTask);
    }
    inFile.close();
    return head;
};