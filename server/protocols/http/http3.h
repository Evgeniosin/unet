#ifndef HTTP3_H
#define HTTP3_H

namespace usub::server::protocols::http {
#if WITH_HTTP3 
    class Http3Context : public HttpContext {
    private:
    protected:
    public:
    };
#endif //WITH_HTTP3

}

#endif //HTTP3_H