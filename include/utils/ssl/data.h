//
// Created by Kirill Zhukov on 16.05.2024.
//

#ifndef SERVER_DATA_H
#define SERVER_DATA_H

#include <vector>
#include <iostream>
#include "openssl/ssl.h"
#include "openssl/err.h"

namespace usub::utils::ssl {
    extern size_t encrypt_data_SSL(SSL *ssl, std::vector<uint8_t> &input_data);
}

#endif //SERVER_DATA_H
