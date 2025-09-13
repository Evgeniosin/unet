cmake_minimum_required(VERSION 3.14)
project(ServerBuildTests)

Find_Package(uvent REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(ServerBuildTests ServerBuildTests.cpp)

target_link_libraries(ServerBuildTests PRIVATE
    uvent
    server
)

target_include_directories(ServerBuildTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/server/server
        ${CMAKE_SOURCE_DIR}/include/Protocols/HTTP
        ${CMAKE_SOURCE_DIR}/include/StreamHandlers
        ${CMAKE_SOURCE_DIR}/include/utils/
        ${CMAKE_SOURCE_DIR}/external/glaze-5.5.4/include
)


if (USE_OPEN_SSL)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(ServerBuildTests PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto)

    target_compile_definitions(ServerBuildTests PRIVATE USE_OPEN_SSL)
endif ()