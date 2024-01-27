#ifndef U_APPLICATION_H
#define U_APPLICATION_H

#include <event2/event.h>
#include <openssl/ssl.h>

namespace usub::server::core {
    class AppContext {
    public:
        AppContext(SSL_CTX *ssl_ctx, event_base *event_base);

        SSL_CTX *ssl_ctx;
        event_base *evbase;
    };
}

#endif // U_APPLICATION_H
