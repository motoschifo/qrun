cmake_minimum_required(VERSION 3.16)
project(qrun LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# find Qt6 libraries
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# start from main.cpp
add_executable(qrun main.cpp)

# add Qt6 libraries
target_link_libraries(qrun PRIVATE Qt6::Widgets)
