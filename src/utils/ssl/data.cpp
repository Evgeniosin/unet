//
// Created by Kirill Zhukov on 16.05.2024.
//

#include "data.h"

namespace usub::utils::ssl {
    size_t encrypt_data_SSL(SSL *ssl, std::vector<uint8_t> &input_data) {
        size_t total_written = 0;
        if (!SSL_write_ex(ssl, input_data.data(), input_data.size(), &total_written)) {
            std::cerr << "SSL_write_ex failed: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
            return -1;
        }

        // Now read the encrypted data back from the SSL's memory BIO
        BIO *wbio = SSL_get_wbio(ssl);
        if (!wbio) {
            std::cerr << "Failed to get write BIO" << std::endl;
            return -1;
        }

        // Determine how much data is available to read
        size_t pending = BIO_ctrl_pending(wbio);
        if (pending > 0) {
            std::vector<uint8_t> encrypted_data(pending);
            int read_bytes = BIO_read(wbio, encrypted_data.data(), encrypted_data.size());
            if (read_bytes <= 0) {
                std::cerr << "Failed to read from BIO" << std::endl;
                return -1;
            }

            // Optionally, replace input_data with encrypted_data
            input_data.swap(encrypted_data);
        }
        return total_written;
    }
}