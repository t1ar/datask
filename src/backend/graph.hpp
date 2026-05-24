#pragma once
#include <iostream>
#include <sstream>
#include "task.hpp"

struct GraphNode;

//Edge node forming a linked list 
//of requirements for a single GraphNode.
struct DependencyEdge {
    GraphNode* DependencyNode;
    DependencyEdge* next = nullptr;
};

//Vertex in the Directed Graph representing a specific Task.
struct GraphNode {
    Task* taskPtr;
    DependencyEdge* outgoingEdge = nullptr;
};

//Wires a directed edge between 
//a dependent task and its requirement.
inline void addDependency (GraphNode* sourceNode, GraphNode* waitingNode){
    DependencyEdge* newEdge = new DependencyEdge{waitingNode,nullptr};
    newEdge->next = sourceNode->outgoingEdge;
    sourceNode->outgoingEdge = newEdge;
};

//Recursively traverses and prints 
//the graph edges to the console.
inline void printDependenciesChain(GraphNode* current, int indentLevel = 0){
    if (!current){
        return;
    }
    for (int i = 0 ; i < indentLevel; i++){
        std::cout << "  ";
    }
    std::cout << "-> " << current->taskPtr->title << "\n";
    DependencyEdge* edgeExplorer = current->outgoingEdge;

    while (edgeExplorer){
        printDependenciesChain(edgeExplorer->DependencyNode, indentLevel+1);
        edgeExplorer = edgeExplorer->next;
    }
};

//Builds the semicolon-separated strings 
//and builds the entire Directed Graph in memory.
inline void buildDependencyGraph(GraphNode** graphArray, int arraySize) {
    for (int i = 0; i < arraySize; i++) {
        if (graphArray[i] == nullptr) continue;
        
        std::string rawDeps = graphArray[i]->taskPtr->rawDependencies;
        if (rawDeps.empty() || rawDeps == "\r" || rawDeps == "\n") continue; 

        std::stringstream ss(rawDeps);
        std::string depID;
        
        while (std::getline(ss, depID, ';')) {
            //Strip invisible characters from the end
            while (!depID.empty() && (depID.back() == '\r' || depID.back() == '\n' || depID.back() == ' ')) {
                depID.pop_back(); 
            }
            //Strip invisible characters from the front
            while (!depID.empty() && depID.front() == ' ') {
                depID.erase(0, 1);
            }

            std::cout << "[LINKING] " << graphArray[i]->taskPtr->id 
                      << " is looking for exactly: '" << depID << "'...\n";
            
            bool found = false;
            for (int j = 0; j < arraySize; j++) {
                if (graphArray[j] != nullptr) {
                    if (graphArray[j]->taskPtr->id == depID) {
                        addDependency(graphArray[i], graphArray[j]);
                        std::cout << "  -> SUCCESS! Edge created.\n";
                        found = true;
                        break;
                    }
                }
            }
            if (!found) {
                std::cout << "  -> FAILED! Could not find Task ID: '" << depID << "' in memory.\n";
            }
        }
    }
};