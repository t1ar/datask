#include "backend/task.hpp"
#include "backend/list.hpp"
#include "backend/hash_table.hpp"
#include "backend/stack.hpp"
#include <iostream>

int main() {
    //1.  init
    ListNode* mainTaskList = nullptr;
    
    // Create our Hash Table array of size 11 and initialize all buckets to nullptr
    HashNode* myHashTable[11] = {nullptr};
    
    // Create our empty Undo Stack tracker
    StackNode* undoStackTop = nullptr;

    std::cout << "--- TASKFLOW BACKEND VERIFICATION ---\n";

    //2. test create tasks
    Task* t1 = new Task{"TSK01", "Design Database Architecture", 1, 20260520};
    Task* t2 = new Task{"TSK02", "Write FTXUI Layout Code", 2, 20260525};

    insertTask(mainTaskList,t1);
    insertTask(mainTaskList,t2);

    insertToTable(myHashTable,t1);
    insertToTable(myHashTable,t2);

    std::cout << "Tasks successfully generated and indexed!\n";

    //3. test lookup
    std::cout << "\nTesting Hash Table Search for ID 'TSK02'...\n";
    
    Task* found = lookupInTable(myHashTable,"TSK02");

    if (found != nullptr) {
        std::cout << "Success! Found Task: " << found->title << "\n";
    } else {
        std::cout << "Error: Task not found in Hash Table.\n";
    }

    //4. test delete and undo
    std::cout << "\nSimulating deletion of 'TSK01' and pushing to Undo Stack...\n";
    
    // For this raw test, we will bypass the list deletion logic and just 
    // simulate pushing t1 to the stack and retrieving it.
    
    // ACTION: Push t1 to your undoStackTop
    push(undoStackTop,t1);
    
    std::cout << "Task pushed to Stack. Triggering manual Undo (Pop)...\n";

    // ACTION: Pop the task back out from undoStackTop and store it in a Task* variable called 'restored'
    Task* restored = pop(undoStackTop);

    if (restored != nullptr) {
        std::cout << "Undo successful! Restored Task: " << restored->title << "\n";
    }

    // Clean list nodes and tasks
    ListNode* current = mainTaskList;
    while (current != nullptr) {
        ListNode* nextNode = current->next;
        delete current->taskPtr; // Free actual Task data
        delete current;          // Free ListNode wrapper
        current = nextNode;
    }

    // Clean Hash Table wrapper nodes (The tasks themselves are already freed above)
    for (int i = 0; i < 11; i++) {
        HashNode* currHash = myHashTable[i];
        while (currHash != nullptr) {
            HashNode* nextHash = currHash->next;
            delete currHash; // Free HashNode wrapper only
            currHash = nextHash;
        }
    }

    std::cout << "\nMemory gracefully swept clean. Systems baseline green!\n";
    return 0;
}