#include "backend/task.hpp"
#include "backend/list.hpp"
#include "backend/hash_table.hpp"
#include "backend/stack.hpp"
#include "backend/queue.hpp"
#include "backend/bst.hpp"
#include "backend/graph.hpp"
#include <iostream>

int main() {
    std::cout << "=== TASKFLOW ULTIMATE SYSTEM TEST ===\n\n";

    // 1. ALLOCATE TASKS
    Task* t1 = new Task{"TSK1", "Design Database", 3, 20260601};
    Task* t2 = new Task{"TSK2", "Write Backend API", 2, 20260610};
    Task* t3 = new Task{"TSK3", "Deploy to Server", 1, 20260620}; // Absolute highest priority!
    Task* t4 = new Task{"TSK4", "Build User Interface", 4, 20260615};

    // ---------------------------------------------------------
    // TEST 1: THE GRAPH ENGINE 
    // ---------------------------------------------------------
    std::cout << "[GRAPH] Building Dependency Chain...\n";
    GraphNode* gn1 = new GraphNode{t1, nullptr};
    GraphNode* gn2 = new GraphNode{t2, nullptr};
    GraphNode* gn3 = new GraphNode{t3, nullptr};
    GraphNode* gn4 = new GraphNode{t4, nullptr};

    // Rule 1: Dont talk about fight club (Database must be done before API and UI)
    addDependency(gn1, gn2);
    addDependency(gn1, gn4);
    // Rule 2: You do not talk about fight club (API must be done before Deployment)
    addDependency(gn2, gn3);

    std::cout << "What happens if we start 'Design Database'?\n";
    printDependenciesChain(gn1, 0);
    std::cout << "\n";

    // ---------------------------------------------------------
    // TEST 2: THE BINARY SEARCH TREE 
    // ---------------------------------------------------------
    std::cout << "[BST] Inserting tasks into Priority Tree...\n";
    BSTNode* priorityRoot = nullptr;
    priorityRoot = insertPriority(priorityRoot, t1);
    priorityRoot = insertPriority(priorityRoot, t2);
    priorityRoot = insertPriority(priorityRoot, t3);
    priorityRoot = insertPriority(priorityRoot, t4);

    Task* mostUrgent = pullHighestPriority(priorityRoot);
    std::cout << "Most Urgent Task: " << mostUrgent->title 
              << " (Priority Level " << mostUrgent->priority << ")\n\n";

    // ---------------------------------------------------------
    // TEST 3: MERGE SORT 
    // ---------------------------------------------------------
    std::cout << "[MERGE SORT] Sorting main list by Deadline...\n";
    ListNode* mainList = nullptr;
    // Inserting them totally out of order!
    insertTask(mainList, t3); 
    insertTask(mainList, t1); 
    insertTask(mainList, t4); 
    insertTask(mainList, t2); 

    // The O(n log n) magic:
    mainList = mergeSort(mainList);

    ListNode* curr = mainList;
    while (curr) {
        std::cout << "Deadline: " << curr->taskPtr->deadline 
                  << " -> " << curr->taskPtr->title << "\n";
        curr = curr->next;
    }

    // ---------------------------------------------------------
    // TEST 4: HASH TABLE SEARCH 
    // ---------------------------------------------------------
    std::cout << "[HASH TABLE] Testing O(1) ID Lookup...\n";
    HashNode* directory[10] = {nullptr};
    insertToTable(directory,t1);
    insertToTable(directory,t2);
    insertToTable(directory,t3);
    insertToTable(directory,t4);
    Task* foundTask = lookupInTable(directory,"TSK2");
    if (foundTask) {
        std::cout << "Found It! " << foundTask->title;
    }
    
    // ---------------------------------------------------------
    // TEST 5: STACK 
    // ---------------------------------------------------------
    std::cout << "\n[STACK] Testing LIFO Undo Engine...\n";
    StackNode* undoStack = nullptr;
    push(undoStack,t1);
    push(undoStack,t2);

    Task* savedTask = pop(undoStack);

    std::cout << "Ooppsie! CTRL+Z! " << savedTask->title;
    
    // ---------------------------------------------------------
    // TEST 6: QUEUE
    // ---------------------------------------------------------
    std::cout << "\n[QUEUE] Testing FIFO Focus Mode...\n";
    TaskQueue dailyQueue;
    enqueue(dailyQueue,t2);
    enqueue(dailyQueue,t3);

    Task* daDequeued = dequeue(dailyQueue);
    std::cout << "Push that mf out! " << daDequeued->title;

    std::cout << "\nALL ENGINES GREEN. ARCHITECTURE VERIFIED!\n";
    return 0;
}