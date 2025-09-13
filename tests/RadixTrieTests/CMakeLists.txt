cmake_minimum_required(VERSION 3.14)
project(RadixTrieTests)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

Find_Package(uvent REQUIRED)

add_executable(RadixTrieTests
    RadixTrieTests.cpp
)

target_link_libraries(RadixTrieTests PRIVATE server uvent)

target_include_directories(RadixTrieTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/Protocols/HTTP
)
