#pragma once
#include "task.hpp"

struct HashNode {
    Task* taskPtr;
    HashNode* next = nullptr;
};

const int HASH_TABLE_SIZE = 11;
HashNode* hashTable[HASH_TABLE_SIZE];

inline int hashFunction(std::string key) {
    int sum = 0;
    for (char c : key) {
        sum += c;
    }
    return sum % HASH_TABLE_SIZE;
};

inline void insertToTable(HashNode* table[], Task* newTask){
    int index = hashFunction(newTask->id);
    HashNode* newNode = new HashNode;

    newNode->taskPtr = newTask;
    
    newNode -> next = table[index];
    table[index] = newNode;
};

inline Task* lookupInTable(HashNode* table[], std::string targetID){
    int index = hashFunction(targetID);

    HashNode* currentNode = table[index];

    while (currentNode) {
        if (currentNode->taskPtr->id == targetID) {
            return currentNode->taskPtr;
        }
        currentNode = currentNode->next;
    } return nullptr;
};