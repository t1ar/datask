#pragma once
#include <iostream>
#include "task.hpp"

struct GraphNode;

struct DependencyEdge {
    GraphNode* DependencyNode;
    DependencyEdge* next = nullptr;
};

struct GraphNode {
    Task* taskPtr;
    DependencyEdge* outgoingEdge = nullptr;
};

inline void addDependency (GraphNode* sourceNode, GraphNode* waitingNode){
    DependencyEdge* newEdge = new DependencyEdge{waitingNode,nullptr};
    newEdge->next = sourceNode->outgoingEdge;
    sourceNode->outgoingEdge = newEdge;
};

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