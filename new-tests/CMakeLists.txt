cmake_minimum_required(VERSION 3.22)

project(new_form_data_tests)

add_executable(test_form_data
    test_form_data.cpp
)

add_executable(test_http1_parser
    test_http1_parser.cpp
)

# Include new headers
target_include_directories(test_form_data PRIVATE
    ${CMAKE_SOURCE_DIR}/new-include
)

target_include_directories(test_http1_parser PRIVATE
    ${CMAKE_SOURCE_DIR}/new-include
)

if(TARGET unet)
    target_link_libraries(test_form_data PRIVATE unet)
endif()

if(TARGET unet)
    target_link_libraries(test_http1_parser PRIVATE unet)
endif()

# Simple test run target
add_custom_target(run_form_data_tests
    COMMAND test_form_data
    DEPENDS test_form_data
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(run_http1_parser_tests
    COMMAND test_http1_parser
    DEPENDS test_http1_parser
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
