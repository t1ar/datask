#include "backend/list.hpp"
#include "backend/storage.hpp"
#include "frontend/dashboard.hpp"

int main(){
    std::string dbFile = "database.csv";
    ListNode* mainList = loadTasks(dbFile);
    if (!mainList){
        insertTask(mainList, new Task{"TSK1","An Empty Task",1,20260522,false,""});
    }
    launchDashboard(mainList);
    return 0;
}

