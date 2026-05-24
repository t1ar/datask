#pragma once
#include "../backend/task.hpp"
#include "../backend/storage.hpp"
#include "../backend/bst.hpp"
#include "../backend/queue.hpp"
#include "../backend/cleanup.hpp"
#include "../backend/stack.hpp"
#include "../backend/graph.hpp"
#include "../backend/list.hpp"
#include "../backend/hash_table.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <vector>
#include <string>

//Here begins the 700+ lines of frontend coding
//Everybody loves designing them, but not programming them
//the LaunchDashboard is a hell of a place, its being
//holded by a thin strings of spaghetti. Have fun reading this.

//Prevent text from leaking out the boxes
inline std::string truncateText(const std::string& str, size_t maxLength = 25) {
    if (str.length() > maxLength) {
        return str.substr(0, maxLength - 3) + "...";
    }
    return str;
};

//Traverses the Doubly Linked List 
//to retrieve a task based on its Menu index.
inline Task* getSelectedTask(ListNode* head,int index){
    ListNode* curr = head;
    for (int i = 0; i < index; i++){
        if (curr){
            curr = curr->next;
        }
    }
    if (curr) {
        return curr->taskPtr;
    } else {
        return nullptr;
    }
};

//Traverses the Doubly Linked List 
//to retrieve a task based on its Menu index.
inline void removeTaskAtIndex(ListNode*& head,int index){
    if (!head){
        return;
    }
    if (index == 0) {
        ListNode* temp = head;
        head = head->next;
        delete temp->taskPtr;
        delete temp;
        return;
    } else {
        ListNode* curr = head;
        for (int i = 0; i < (index - 1); i++){
            if (!curr || !curr->next){
                return;
            } curr = curr->next;
        }
        ListNode* nodeToDelete = curr->next;
        curr->next = nodeToDelete->next;
        delete nodeToDelete->taskPtr;
        delete nodeToDelete;
    }
};

//SYNC ALL OF ITTTT
//destory all the data and then rebuild it. 
inline void syncAllEngines(ListNode* mainList, BSTNode*& root, TaskQueue& queue,
                            HashNode* table[], GraphNode**& graphArray, int& graphSize){
    // 1. Destroy the old engines
    freeBST(root);
    root = nullptr;

    while(queue.front != nullptr) {
        dequeue(queue); // Safely empty the queue
    }

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* currHash = table[i];
        while (currHash) {
            HashNode* temp = currHash;
            currHash = currHash->next;
            delete temp;
        }
        table[i] = nullptr;
    }

    if (graphArray != nullptr) {
        for (int i = 0; i < graphSize; i++) {
            if (graphArray[i]) freeGraph(graphArray[i]);
        }
        delete[] graphArray;
        graphArray = nullptr;
    }

    // 2. Measure the new database size
    graphSize = 0;
    ListNode* counter = mainList;
    while(counter) { 
        graphSize++; 
        counter = counter->next; 
    }

    // 3. Rebuild all Engines
    if (graphSize > 0) {
        graphArray = new GraphNode*[graphSize];
    }

    ListNode* curr = mainList;
    int gIndex = 0;
    
    while (curr) {
        Task* t = curr->taskPtr;

        // A. Sync BST & Queue (ONLY active tasks)
        if (!t->completed) {
            root = insertPriority(root, t);
            enqueue(queue, t);
        }

        // B. Sync Hash Table (ALL tasks)
        insertToTable(table, t);

        // C. Sync Graph Nodes (ALL tasks)
        graphArray[gIndex] = new GraphNode{t, nullptr};
        gIndex++;

        curr = curr->next;
    }

    // 4. Connects the Graph Edges based on dependencies
    if (graphSize > 0) {
        buildDependencyGraph(graphArray, graphSize);
    }
}

//SYNC IT AGAINNNN
//Sync the vectors FTXUI with the SyncEngines on top.
inline void syncUIState(ListNode* mainList, std::vector<std::string>& taskEntries, int& selectedTask, 
                        std::vector<std::string>& depTitles, std::vector<std::string>& depIDs, 
                        std::vector<bool>& depStates, int& selectedDepIndex) {
    // 1. Rebuild the Main Menu 
    taskEntries.clear();
    ListNode* curr = mainList;
    while(curr) {
        std::string prefix = curr->taskPtr->completed ? "[X] " : "[ ] ";
        taskEntries.push_back(prefix + curr->taskPtr->title);
        curr = curr->next;
    }
    
    if (taskEntries.empty()) { selectedTask = 0; } 
    else if (selectedTask >= taskEntries.size()) { selectedTask = taskEntries.size() - 1; }

    // 2. Rebuild the MULTI-SELECT Dependency Dropdown
    depTitles.clear();
    depIDs.clear();
    depStates.clear();
    
    curr = mainList;
    while(curr) {
        depTitles.push_back("[ ] " + curr->taskPtr->title); // Start unchecked
        depIDs.push_back(curr->taskPtr->id);
        depStates.push_back(false); // Start false
        curr = curr->next;
    }
    selectedDepIndex = 0; 
}

//Generate a "TSKXX" id
inline std::string generateNextID(ListNode* head) {
    int maxNumber = 0;
    ListNode* curr = head;
    
    while (curr) {
        std::string currentID = curr->taskPtr->id;
        
        // Check if the ID starts with "TSK" (e.g., TSK1, TSK42)
        if (currentID.length() > 3 && currentID.substr(0, 3) == "TSK") {
            try {
                // Extract the number part and convert to integer
                int num = std::stoi(currentID.substr(3));
                if (num > maxNumber) {
                    maxNumber = num;
                }
            } catch (...) {
                // Ignore weirdly formatted IDs
            }
        }
        curr = curr->next;
    }
    
    // Return "TSK" plus the next highest number!
    return "TSK" + std::to_string(maxNumber + 1);
}

//Here we go gents, NO GOING BACK
inline void launchDashboard(ListNode* mainList){
    
    // ==========================================
    // 1. MASTER APPLICATION STATE
    // ==========================================
    bool isEditing = false;
    Task* taskBeingEdited = nullptr;

    std::string dbFile = "database.csv";
    std::string headerText = " DaTask Dashboard ";
    
    BSTNode* bstRoot = nullptr;
    TaskQueue focusQueue;
    StackNode* undoStack = nullptr;
    HashNode* appHashTable[HASH_TABLE_SIZE] = {nullptr};
    
    GraphNode** graphArray = nullptr;
    int graphSize = 0;

    // Boot Initialization
    syncAllEngines(mainList, bstRoot, focusQueue, appHashTable, graphArray, graphSize);
    
    std::string inputTitle = "";
    std::string inputPriority = "";
    std::string inputDeadline = "";
    std::string searchInput = "";

    std::vector<std::string> depTitles;
    std::vector<std::string> depIDs;
    std::vector<bool> depStates;

    std::vector<std::string> taskEntries = {};
    int selectedDepIndex = 0;
    int selectedTask = 0;
    
    syncUIState(mainList, taskEntries, selectedTask, depTitles, depIDs, depStates, selectedDepIndex);

    // ==========================================
    // 2. UI COMPONENT LOGIC & LAMBDAS
    // ==========================================
    ftxui::MenuOption depMenuOption;
    depMenuOption.on_enter = [&] {
        if (depStates.empty()) return;
        
        depStates[selectedDepIndex] = !depStates[selectedDepIndex];
        std::string rawTitle = depTitles[selectedDepIndex].substr(4); 
        depTitles[selectedDepIndex] = (depStates[selectedDepIndex] ? "[X] " : "[ ] ") + rawTitle;
    };
    ftxui::Component depSelector = ftxui::Menu(&depTitles, &selectedDepIndex, depMenuOption);
    
    // Centralized logic for completing tasks and checking Graph req.
    auto toggleTaskLogic = [&] {
        Task* activeTask = getSelectedTask(mainList, selectedTask);
        if (!activeTask) return;

        // BRANCH A: THE FORWARD BLOCKER
        if (!activeTask->completed) {
            bool canComplete = true;
            std::string missingDeps = "";

            GraphNode* thisNode = nullptr;
            if (graphArray != nullptr) {
                for (int i = 0; i < graphSize; i++) {
                    if (graphArray[i] != nullptr && graphArray[i]->taskPtr == activeTask) {
                        thisNode = graphArray[i];
                        break;
                    }
                }
            }

            if (thisNode != nullptr) {
                DependencyEdge* edge = thisNode->outgoingEdge;
                while (edge != nullptr) {
                    if (edge->DependencyNode != nullptr && edge->DependencyNode->taskPtr != nullptr) {
                        if (!edge->DependencyNode->taskPtr->completed) {
                            canComplete = false;
                            missingDeps += edge->DependencyNode->taskPtr->id + " ";
                        }
                    }
                    edge = edge->next;
                }
            }

            if (!canComplete) {
                headerText = " [ ACCESS DENIED: Complete " + missingDeps + "first! ] ";
                return; 
            }
        }
        // BRANCH B: THE REVERSE BLOCKER 
        else {
            bool canUncheck = true;
            std::string blockingTasks = "";

            if (graphArray != nullptr) {
                // Scan every task in the graph
                for (int i = 0; i < graphSize; i++) {
                    if (graphArray[i] != nullptr && graphArray[i]->taskPtr->completed) {
                        DependencyEdge* edge = graphArray[i]->outgoingEdge;
                        while (edge != nullptr) {
                            if (edge->DependencyNode != nullptr && edge->DependencyNode->taskPtr == activeTask) {
                                canUncheck = false;
                                blockingTasks += graphArray[i]->taskPtr->id + " ";
                            }
                            edge = edge->next;
                        }
                    }
                }
            }

            if (!canUncheck) {
                headerText = " [ DENIED: " + blockingTasks + "rely on this. Un-check them first! ] ";
                return;
            }
        }

        // ==========================================
        // NORMAL COMPLETION LOGIC
        // ==========================================
        activeTask->completed = !activeTask->completed;
        headerText = activeTask->completed ? " [ TASK COMPLETED! ] " : " [ TASK UN-CHECKED ] ";

        saveTasks(mainList, dbFile);
        syncUIState(mainList, taskEntries, selectedTask, depTitles, depIDs, depStates, selectedDepIndex);
        syncAllEngines(mainList, bstRoot, focusQueue, appHashTable, graphArray, graphSize);
    };

    ftxui::MenuOption option;
    option.on_enter = toggleTaskLogic;
    
    ftxui::Component menu = ftxui::Menu(&taskEntries, &selectedTask, option);

    ftxui::Component titleBox = ftxui::Input(&inputTitle, "Task Title...");
    ftxui::Component priorityBox = ftxui::Input(&inputPriority, "Priority (e.g. 1)...");
    ftxui::Component deadlineBox = ftxui::Input(&inputDeadline, "Deadline (YYYYMMDD)...");
    ftxui::Component searchBox = ftxui::Input(&searchInput, "Search ID (e.g. TSK1)...");
    
    ftxui::Component searchBtn = ftxui::Button("Find Task", [&] {
        if (searchInput.empty()) return;

        Task* foundTask = lookupInTable(appHashTable, searchInput);
        
        if (foundTask) {
            int index = 0;
            ListNode* curr = mainList;
            while (curr) {
                if (curr->taskPtr->id == searchInput) {
                    selectedTask = index; 
                    headerText = " [ TASK FOUND! ] ";
                    break;
                }
                curr = curr->next;
                index++;
            }
        } else {
            headerText = " [ ERROR: TASK ID NOT FOUND ] ";
        }
        searchInput = ""; 
    });

    ftxui::Component searchForm = ftxui::Container::Vertical({searchBox, searchBtn});

    ftxui::Component editBtn = ftxui::Button("Edit Selected", [&] {
        Task* activeTask = getSelectedTask(mainList, selectedTask);
        if (!activeTask) return;

        isEditing = true;
        taskBeingEdited = activeTask;

        inputTitle = activeTask->title;
        inputPriority = std::to_string(activeTask->priority);
        inputDeadline = std::to_string(activeTask->deadline);

        selectedDepIndex = 0;
        
        for (size_t i = 0; i < depStates.size(); i++) {
            depStates[i] = false;
            std::string rawTitle = depTitles[i].substr(4);
            depTitles[i] = "[ ] " + rawTitle;
        }

        std::string rawDeps = activeTask->rawDependencies;
        std::stringstream ss(rawDeps);
        std::string depID;
        while (std::getline(ss, depID, ';')) {
            while (!depID.empty() && (depID.back() == '\r' || depID.back() == '\n' || depID.back() == ' ')) depID.pop_back();
            while (!depID.empty() && depID.front() == ' ') depID.erase(0, 1);
            
            for (size_t i = 0; i < depIDs.size(); i++) {
                if (depIDs[i] == depID) {
                    depStates[i] = true;
                    std::string rawTitle = depTitles[i].substr(4);
                    depTitles[i] = "[X] " + rawTitle;
                    break;
                }
            }
        }
        headerText = " [ EDIT MODE: Modifying " + activeTask->id + " ] ";
    });

    ftxui::Component submitBtn = ftxui::Button("Submit (Create/Save)", [&] {
        if (inputTitle.empty()) return;

        int prio = 1; 
        if (!inputPriority.empty()) { try { prio = std::stoi(inputPriority); } catch (...) {} }
        
        int dead = 0; 
        if (!inputDeadline.empty()) { try { dead = std::stoi(inputDeadline); } catch (...) {} }
        
        std::string actualDependencyID = "";
        for (size_t i = 0; i < depStates.size(); i++) {
            if (depStates[i]) {
                if (isEditing && taskBeingEdited != nullptr && depIDs[i] == taskBeingEdited->id) {
                    headerText = " [ ERROR: A task cannot depend on itself! ] ";
                    return; 
                }
                actualDependencyID += depIDs[i] + ";";
            }
        }
        
        if (!actualDependencyID.empty()) {
            actualDependencyID.pop_back();
        }

        if (isEditing && taskBeingEdited != nullptr) {
            taskBeingEdited->title = inputTitle;
            taskBeingEdited->priority = prio;
            taskBeingEdited->deadline = dead;
            taskBeingEdited->rawDependencies = actualDependencyID;
            
            std::string prefix = taskBeingEdited->completed ? "[X] " : "[ ] ";
            taskEntries[selectedTask] = prefix + taskBeingEdited->title;
            
            headerText = " [ TASK UPDATED SUCESSFULLY! ] ";
            
            isEditing = false;
            taskBeingEdited = nullptr;
        } 
        else {
            std::string uniqueID = generateNextID(mainList);
            Task* newTask = new Task{uniqueID, inputTitle, prio, dead, false, actualDependencyID};
            insertTask(mainList, newTask);
            
            taskEntries.push_back("[ ] " + inputTitle);
            headerText = " [ TASK CREATED SUCESSFULLY! ] ";
        }

        saveTasks(mainList, dbFile);
        
        inputTitle = "";
        inputPriority = "";
        inputDeadline = "";
        
        syncUIState(mainList, taskEntries, selectedTask, depTitles, depIDs, depStates, selectedDepIndex);
        syncAllEngines(mainList, bstRoot, focusQueue, appHashTable, graphArray, graphSize);
    });


    ftxui::Component deleteBtn = ftxui::Button("Delete Task", [&] {
        if (taskEntries.empty()) return;
        
        Task* taskToTrash = getSelectedTask(mainList, selectedTask);
        if (taskToTrash) {
            Task* clonedTask = new Task {
                taskToTrash->id, taskToTrash->title, taskToTrash->priority,
                taskToTrash->deadline, taskToTrash->completed, taskToTrash->rawDependencies
            };
            push(undoStack, clonedTask);
        }
        
        removeTaskAtIndex(mainList, selectedTask);
        
        saveTasks(mainList, dbFile);
        syncUIState(mainList, taskEntries, selectedTask, depTitles, depIDs, depStates, selectedDepIndex);
        syncAllEngines(mainList, bstRoot, focusQueue, appHashTable, graphArray, graphSize);
        if (graphSize > 0 && graphArray[0] != nullptr) {
            std::cout << "\n--- CURRENT GRAPH STATE ---\n";
            printDependenciesChain(graphArray[0], 0);
        }
    });

    ftxui::Component toggleBtn = ftxui::Button("Toggle Complete", toggleTaskLogic);
    
    ftxui::Component sortDeadBtn = ftxui::Button("Sort: Deadline", [&] {
        if (!mainList) return;
        mainList = mergeSort(mainList, false); 
        selectedTask = 0;
        saveTasks(mainList, dbFile);
        syncUIState(mainList, taskEntries, selectedTask, depTitles, depIDs, depStates, selectedDepIndex);
        syncAllEngines(mainList, bstRoot, focusQueue, appHashTable, graphArray, graphSize);
        headerText = " [ LIST SORTED BY DEADLINE! ] ";
    });

    ftxui::Component sortPrioBtn = ftxui::Button("Sort: Priority", [&] {
        if (!mainList) return;
        mainList = mergeSort(mainList, true); 
        selectedTask = 0;
        saveTasks(mainList, dbFile);
        syncUIState(mainList, taskEntries, selectedTask, depTitles, depIDs, depStates, selectedDepIndex);
        syncAllEngines(mainList, bstRoot, focusQueue, appHashTable, graphArray, graphSize);
        headerText = " [ LIST SORTED BY PRIORITY! ] ";
    });

    ftxui::Component buttonRow1 = ftxui::Container::Horizontal({submitBtn, editBtn, deleteBtn});
    ftxui::Component buttonRow2 = ftxui::Container::Horizontal({toggleBtn, sortDeadBtn, sortPrioBtn});

    ftxui::Component creationForm = ftxui::Container::Vertical({
        titleBox, priorityBox, deadlineBox, depSelector, buttonRow1, buttonRow2 
    }); 

    // ==========================================
    // 3. UI RENDERERS (Visual Layout Boxes)
    // ==========================================
    ftxui::Component rightPane = ftxui::Renderer([&]() -> ftxui::Element {
        Task* activeTask = getSelectedTask(mainList, selectedTask);
        
        if (activeTask == nullptr) return ftxui::text("No task selected.");
        
        std::string prioStr = std::to_string(activeTask->priority);
        std::string deadStr = std::to_string(activeTask->deadline);
        
        std::string depStr = activeTask->rawDependencies;
        if (depStr.empty() || depStr == "\r" || depStr == "\n") {
            depStr = "None";
        }

        return ftxui::vbox({
            ftxui::window(ftxui::text(" DETAILS "), ftxui::vbox({
                ftxui::text("ID: " + activeTask->id) | ftxui::bold,
                ftxui::text("Title: " + activeTask->title),
                ftxui::text("Deadline: " + deadStr)
            })),
            ftxui::window(ftxui::text(" PRIORITY "), ftxui::text(prioStr)),
            ftxui::window(ftxui::text(" GRAPH DEPENDENCIES "), ftxui::text(depStr))
        }) | ftxui::flex; 
    });

    ftxui::Component telemetryPane = ftxui::Renderer([&]() -> ftxui::Element {
        Task* topPriority = pullHighestPriority(bstRoot); 
        Task* nextUp = focusQueue.front ? focusQueue.front->taskPtr : nullptr; 

        std::string topTitle = topPriority ? topPriority->title : "No Active Tasks!";
        std::string queueTitle = nextUp ? nextUp->title : "Queue Empty";

        return ftxui::window(ftxui::text(" TELEMETRY "), ftxui::vbox({
            ftxui::hbox({ ftxui::text("[!] Top Priority: " + truncateText(topTitle)) | ftxui::color(ftxui::Color::RedLight), ftxui::filler() }),
            ftxui::separator(),
            ftxui::hbox({ ftxui::text("[>] Next in Queue: " + truncateText(queueTitle)) | ftxui::color(ftxui::Color::YellowLight), ftxui::filler() })
        }));
    });

    // ==========================================
    // 4. MASTER LAYOUT & FLEXBOX ENGINE
    // ==========================================
    ftxui::Component masterContainer = ftxui::Container::Horizontal({menu, rightPane, creationForm, searchForm});

    ftxui::Component renderer = ftxui::Renderer(masterContainer, [&]{
        
        auto terminalSize = ftxui::Terminal::Size();
        if (terminalSize.dimx < 90 || terminalSize.dimy < 25) {
            return ftxui::window(
                ftxui::text(" TERMINAL TOO SMALL ") | ftxui::bold | ftxui::color(ftxui::Color::RedLight),
                ftxui::text("Please maximize your terminal window to use TaskFlow.")
            ) | ftxui::center;
        }

        return ftxui::border(
            ftxui::vbox({
                ftxui::text(headerText) | ftxui::bold | ftxui::center,
                ftxui::separator(),
                ftxui::hbox({   
                    
                    // --- LEFT COLUMN (50% Split) ---
                    ftxui::vbox({
                        menu->Render() | ftxui::vscroll_indicator | ftxui::frame | ftxui::flex, 
                        telemetryPane->Render() | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 7),
                        ftxui::separator(),
                        ftxui::window(ftxui::text(" DATABASE SEARCH "), ftxui::vbox({
                            searchBox->Render(),
                            ftxui::hbox({ searchBtn->Render(), ftxui::filler() })
                        }))
                    }) | ftxui::flex, 
                    
                    ftxui::separator(),
                    
                    // --- RIGHT COLUMN (50% Split) ---
                    ftxui::vbox({
                        rightPane->Render(),
                        ftxui::filler(), 
                        ftxui::separator(),
                        ftxui::window(ftxui::text(" CREATE NEW TASK "), ftxui::vbox({
                            titleBox->Render(),
                            priorityBox->Render(),
                            deadlineBox->Render(),
                            ftxui::window(ftxui::text(" Requires: "), 
                                depSelector->Render() | ftxui::vscroll_indicator | ftxui::frame | ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, 5)
                            ),
                            buttonRow1->Render(),
                            buttonRow2->Render(),
                        }))
                    }) | ftxui::flex 
                    
                }) | ftxui::flex 
            }) | ftxui::flex 
        );
    });

    // ==========================================
    // 5. EVENT HANDLERS & MASTER LOOP
    // ==========================================
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    ftxui::Component appWithUndo = ftxui::CatchEvent(renderer, [&](ftxui::Event event) {
        
        // HOTKEY: UNDO STACK (Ctrl+Z)
        if (event == ftxui::Event::CtrlZ) {
            headerText = " [ CHECKING UNDO STACK... ] ";
            Task* restoredTask = pop(undoStack);
            
            if (restoredTask) {
                headerText = " [ UNDO SUCCESSFUL! ] ";
                insertTask(mainList, restoredTask);
                
                std::string prefix = restoredTask->completed ? "[X] " : "[ ] ";
                taskEntries.push_back(prefix + restoredTask->title);
                
                saveTasks(mainList, dbFile);
                syncUIState(mainList, taskEntries, selectedTask, depTitles, depIDs, depStates, selectedDepIndex);
                syncAllEngines(mainList, bstRoot, focusQueue, appHashTable, graphArray, graphSize);
                
                return true; 
            } else {
                headerText = " [ ERROR: UNDO STACK IS EMPTY! ] ";
                return true;
            }
        } 
        // HOTKEY: SORT BY DEADLINE (Ctrl+D)
        else if (event == ftxui::Event::CtrlD) {
            if (!mainList) return false;
            mainList = mergeSort(mainList, false); 

            taskEntries.clear();
            ListNode* curr = mainList;
            while(curr) {
                std::string prefix = curr->taskPtr->completed ? "[X] " : "[ ] ";
                taskEntries.push_back(prefix + curr->taskPtr->title);
                curr = curr->next;
            }
            selectedTask = 0;
            saveTasks(mainList, dbFile);
            syncUIState(mainList, taskEntries, selectedTask, depTitles, depIDs, depStates, selectedDepIndex);
            syncAllEngines(mainList, bstRoot, focusQueue, appHashTable, graphArray, graphSize);
            
            headerText = " [ LIST SORTED BY DEADLINE! ] ";
            return true;
        }
        // HOTKEY: SORT BY PRIORITY (Ctrl+P)
        else if (event == ftxui::Event::CtrlP) {
            if (!mainList) return false;
            mainList = mergeSort(mainList, true); 

            taskEntries.clear();
            ListNode* curr = mainList;
            while(curr) {
                std::string prefix = curr->taskPtr->completed ? "[X] " : "[ ] ";
                taskEntries.push_back(prefix + curr->taskPtr->title);
                curr = curr->next;
            }
            selectedTask = 0;
            saveTasks(mainList, dbFile);
            syncUIState(mainList, taskEntries, selectedTask, depTitles, depIDs, depStates, selectedDepIndex);
            syncAllEngines(mainList, bstRoot, focusQueue, appHashTable, graphArray, graphSize);
            
            headerText = " [ LIST SORTED BY PRIORITY! ] ";
            return true;
        }
        // HOTKEY: CLEAN QUIT (Ctrl+Q)
        else if (event == ftxui::Event::CtrlQ) {
            screen.ExitLoopClosure()(); 
            return true;
        }
        return false; 
    });
    
    screen.Loop(appWithUndo);

    // ==========================================
    // 6. SAFE EXIT CLEANUP & TERMINAL LOGGING
    // ==========================================
    if (graphSize > 0 && graphArray != nullptr && graphArray[0] != nullptr) {
        std::cout << "\n===================================\n";
        std::cout << "   FINAL TASK DEPENDENCY GRAPH     \n";
        std::cout << "===================================\n";
        printDependenciesChain(graphArray[0], 0);
        std::cout << "\n";
    }
    
    // De-allocate all dynamically mapped heap memory
    freeList(mainList);

    std::cout << "Press ENTER to close the terminal...";
    std::string waitInput;
    std::getline(std::cin, waitInput);
};