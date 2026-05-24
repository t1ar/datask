#pragma once
#include "task.hpp"

//Node for handling collisions in the 
//Hash Table via separate chaining (linked lists).
struct HashNode {
    Task* taskPtr;
    HashNode* next = nullptr;
};

const int HASH_TABLE_SIZE = 1009; // no way the hash is not gonna work
HashNode* hashTable[HASH_TABLE_SIZE];

//Generates an array index 
//from a Task ID using ASCII value summation.
inline int hashFunction(std::string key) {
    int sum = 0;
    for (char c : key) {
        sum += c;
    }
    return sum % HASH_TABLE_SIZE;
};

//Hashes a task and places it into the table, 
//managing collisions via chaining.
inline void insertToTable(HashNode* table[], Task* newTask){
    int index = hashFunction(newTask->id);
    HashNode* newNode = new HashNode;

    newNode->taskPtr = newTask;
    
    newNode -> next = table[index];
    table[index] = newNode;
};

//Retrieves a task in O(1) average time based on its ID.
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