cmake_minimum_required(VERSION 3.10)

project(EcondingTests)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(
    ../../include
)

add_executable(PunycodeEncodingTests
    PunycodeEncodingTests.cpp
    ../../src/Components/Encodings/PunycodeEncoding.cpp
    ../../include/Components/Encodings/PunycodeEncoding.h
)

if (MSVC)
    # For Microsoft Visual C++
    target_compile_options(PunycodeEncodingTests PRIVATE "/U NDEBUG")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    # For GCC, Clang, and AppleClang
    target_compile_options(PunycodeEncodingTests PRIVATE "-UNDEBUG")
else()
    message(WARNING "Compiler does not support undefining NDEBUG automatically. Asserts may be disabled.")
endif()

# Enable testing in this subdirectory
enable_testing()

# Add a test for the format_tests executable
# add_test(NAME FormatTests COMMAND format_tests)
# add_test(NAME MessageTests COMMAND message_tests)
#add_test(NAME Fail COMMAND fail)
add_test(NAME PunycodeEncodingTests COMMAND PunycodeEncodingTests)

