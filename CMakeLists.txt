cmake_minimum_required(VERSION 3.14)
project(DaTask VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG v6.1.9 # Uses latest stable version 6.1.9
)

FetchContent_MakeAvailable(ftxui)

# define src
add_executable(DaTask
    src/main.cpp
)

# ftxui links
target_link_libraries(DaTask PRIVATE 
    ftxui::screen 
    ftxui::dom 
    ftxui::component
)

add_custom_command(TARGET DaTask POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/database_ex.csv"
    "$<TARGET_FILE_DIR:DaTask>/database.csv"
    COMMENT "Copying example database.csv to the output directory..."
)