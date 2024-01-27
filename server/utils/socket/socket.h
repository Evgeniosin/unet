//
// Created by Kirill Zhukov on 09.01.2024.
//

#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "core/net.h"
#include "logging/logger.h"

namespace usub::server::net {
    int createSocket(int port, const std::string &ip_addr, int backlog, usub::server::core::IPV ipv);
}
#endif //SOCKET_H
