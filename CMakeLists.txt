cmake_minimum_required(VERSION 3.20)
project(rbtree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} main.cpp rbtree.cpp)
