cmake_minimum_required(VERSION 3.22)

project(B_plus_tree_visualization)

find_package(Qt5 REQUIRED COMPONENTS Widgets Gui)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_BUILD_TYPE "Debug")


include_directories(source)

FILE(GLOB SOURCES source/*.cpp)
FILE(GLOB SOURCES_TREEMODEL source/TreeModel/*.cpp)

add_executable(
        B_plus_tree_visualization
        ${SOURCES}
        ${SOURCES_TREEMODEL}
)

target_link_libraries(B_plus_tree_visualization Qt5::Widgets Qt5::Gui)
