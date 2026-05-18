cmake_minimum_required(VERSION 3.14)
project(TaskFlow VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG v5.0.0 # Uses stable version 5.0.0
)

FetchContent_MakeAvailable(ftxui)

# define src
add_executable(taskflow 
    src/main.cpp 
    src/backend/task.hpp
    src/backend/list.hpp
    src/backend/hash_table.hpp
    src/backend/stack.hpp
)

# ftxui links
target_link_libraries(taskflow PRIVATE 
    ftxui::screen 
    ftxui::dom 
    ftxui::component
)