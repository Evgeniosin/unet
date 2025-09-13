#pragma once

#include <openssl/ssl.h>
#include <openssl/err.h>
// #include "logging/logger.h"

namespace usub::utils::ssl
{

SSL_CTX* create_http1_ctx(const char* key_file, const char* cert_file)
{
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file (ctx, key_file,  SSL_FILETYPE_PEM);

    return ctx;
}

SSL* create_ssl(SSL_CTX* ctx)
{
    SSL *ssl;
    ssl = SSL_new(ctx);
    if (!ssl) {
        // error_log("Could not create SSL/TLS session object: %s",
             // ERR_error_string(ERR_get_error(), nullptr));
    }
    return ssl;
}


} // namespace usub::utils::ssl