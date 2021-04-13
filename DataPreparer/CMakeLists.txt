cmake_minimum_required(VERSION 3.17)
Project(DataPreparer)

add_executable(PrepareData main.cpp trie_preparer.cpp)

add_library(${PROJECT_NAME} trie_preparer.cpp)