cmake_minimum_required(VERSION 3.22)
project(server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

include_directories(
        utils
)

include_directories(${server_SOURCE_DIR})

add_definitions(
        -DDEBUG_HEADERS=1
	-DDEBUG_EXTENDED_INFO=0
)

add_executable(server main.cpp
        core/application.cpp
        core/net.cpp 
        protocols/http/http_base.cpp 
        protocols/http/http_basic_endpoint_handler.cpp 
        protocols/http/http_message.cpp
        protocols/http/http1.cpp 
        protocols/http/http2.cpp 
        server/server.cpp
        utils/configuration/ConfigReader.cpp 
        utils/socket/socket.cpp
        utils/toml/toml.cpp 
        utils/globals.cpp
        core/application.h
        core/net.h 
        protocols/http/http_base.h
        protocols/http/http_basic_endpoint_handler.h
        protocols/http/http_endpoint_handler.h
        protocols/http/http_handler_module.h
        protocols/http/http_message.h
        protocols/http/http1.h  
        protocols/http/http2.h 
        server/server.h 
        utils/configuration/ConfigReader.h
        utils/logging/logger.h 
        utils/ssl/CTX_util.h 
        utils/socket/socket.h
        utils/toml/toml.hpp
        utils/globals.h
)

set(CMAKE_PREFIX_PATH "/root/libs/libevent-2.1.12-stable/build/")
find_package(OpenSSL REQUIRED)
find_package(Libevent REQUIRED)

target_link_libraries(${PROJECT_NAME}
        OpenSSL::SSL
	libevent::core
        libevent::extra
        libevent::openssl
        libevent::pthreads
        /usr/local/lib/libnghttp2.a
	pthread
)
