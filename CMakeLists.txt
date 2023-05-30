cmake_minimum_required(VERSION 3.15)

project(POCO-SERVER)

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server 
  main.cpp
)

add_subdirectory(database)
add_subdirectory(util)
add_subdirectory(service)

target_include_directories(server PRIVATE "${CMAKE_BINARY_DIR}")
target_include_directories(server
  PUBLIC
    ${CMAKE_PROJECT_DIR}/database/.      
    ${CMAKE_PROJECT_DIR}/service/.
    ${CMAKE_PROJECT_DIR}/server/.
    ${CMAKE_PROJECT_DIR}/util/.
)

target_link_libraries(server 
  PRIVATE 
    ${CMAKE_THREAD_LIBS_INIT} 
    ${Poco_LIBRARIES}
    "PocoData"
    "PocoDataMySQL"
)


