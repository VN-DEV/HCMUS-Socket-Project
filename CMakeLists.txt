cmake_minimum_required(VERSION 3.16)
project(HCMUS_LAN_Chat VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Threads REQUIRED)

file(GLOB_RECURSE PROJECT_SOURCES
    "src/*.cpp"
    "src/*.h"
)

add_executable(lan_chat ${PROJECT_SOURCES})
target_include_directories(lan_chat PRIVATE src)

target_link_libraries(lan_chat PRIVATE 
    Qt6::Widgets 
    Threads::Threads
)

if(WIN32)
    target_link_libraries(lan_chat PRIVATE ws2_32)
endif()

