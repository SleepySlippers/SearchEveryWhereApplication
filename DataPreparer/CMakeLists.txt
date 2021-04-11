cmake_minimum_required(VERSION 3.17)
Project(DataPreparer)

add_executable(PrepareData preparer.cpp preparer.h main.cpp)

add_library(${PROJECT_NAME} preparer.cpp preparer.h)