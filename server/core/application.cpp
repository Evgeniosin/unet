#include "application.h"

namespace usub::server::core {
    AppContext::AppContext(SSL_CTX *ssl_ctx, event_base *event_base) : ssl_ctx(ssl_ctx), evbase(event_base) {
    }
}