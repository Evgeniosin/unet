#include <iostream>
#include <cstdlib>
#include "Protocols/HTTP/RadixRouter.h"

// Ваш макрос для тестовых проверок
#define TEST_ASSERT(condition, message, expected, actual)       \
    do {                                                        \
        if (!(condition)) {                                     \
            std::cerr << "Assertion failed: " << message       \
                      << "\n    Expected: " << expected        \
                      << "\n    Actual:   " << actual          \
                      << "\n    at " << __FILE__               \
                      << ":" << __LINE__ << std::endl;         \
            std::exit(1);                                       \
        }                                                       \
    } while (0)

using namespace usub::server::protocols::http;

Request makeRequest(const std::string& method, const std::string& path) {
    Request req;
    req.getRequestMethod() = method;
    req.getURL() = path;
    return req;
}

int main() {
    {
        RadixRouter router;
        router.addHandler({"GET"}, "/",    nullptr, {});
        router.addHandler({"GET"}, "/foo", nullptr, {});

        auto a = makeRequest("GET", "/");
        auto b = makeRequest("GET", "/foo");
        auto c = makeRequest("GET", "/bar");

        TEST_ASSERT(router.match(a).has_value(),
                    "match(/) must succeed", true, router.match(a).has_value());
        TEST_ASSERT(router.match(b).has_value(),
                    "match(/foo) must succeed", true, router.match(b).has_value());
        TEST_ASSERT(!router.match(c).has_value(),
                    "match(/bar) must fail", false, router.match(c).has_value());
    }

    {
        RadixRouter router;
        router.addHandler({"GET"}, "/{id}", nullptr, {});

        auto a = makeRequest("GET", "/123");
        auto b = makeRequest("GET", "/abc");

        TEST_ASSERT(router.match(a).has_value(),
                    "match(/123) must succeed", true, router.match(a).has_value());
        TEST_ASSERT(router.match(b).has_value(),
                    "match(/abc) must succeed", true, router.match(b).has_value());
    }

    {
        RadixRouter router;
        param_constraint onlyDigits{ R"([0-9]+)", "" };
        router.addHandler({"GET"}, "/user/{id}", nullptr, {{"id",&onlyDigits}});

        auto a = makeRequest("GET", "/user/123");
        auto b = makeRequest("GET", "/user/abc");

        TEST_ASSERT(router.match(a).has_value(),
                    "match(/user/123) must succeed", true, router.match(a).has_value());
        TEST_ASSERT(!router.match(b).has_value(),
                    "match(/user/abc) must fail", false, router.match(b).has_value());
    }

    {
        RadixRouter router;
        router.addHandler({"GET"}, "/static/*", nullptr, {});
        router.addHandler({"GET"}, "/files/*",  nullptr, {});

        auto a = makeRequest("GET", "/static/js");
        auto b = makeRequest("GET", "/static");
        auto c = makeRequest("GET", "/files/a/b/c");
        auto d = makeRequest("GET", "/other/files/x");

        TEST_ASSERT(router.match(a).has_value(),
                    "match(/static/js) must succeed", true, router.match(a).has_value());
        TEST_ASSERT(!router.match(b).has_value(),
                    "match(/static) must fail", false, router.match(b).has_value());
        TEST_ASSERT(router.match(c).has_value(),
                    "match(/files/a/b/c) must succeed", true, router.match(c).has_value());
        TEST_ASSERT(!router.match(d).has_value(),
                    "match(/other/files/x) must fail", false, router.match(d).has_value());
    }

    {
        RadixRouter router;
        router.addHandler({"GET"}, "/foo/*/bar/{id}", nullptr, {});

        auto good  = makeRequest("GET", "/foo/x/bar/123");
        auto good1 = makeRequest("GET", "/foo/x/x/123");
        auto good2 = makeRequest("GET", "/foo/x/bar/");

        TEST_ASSERT(router.match(good).has_value(),
                    "match(/foo/x/bar/123) must succeed", true, router.match(good).has_value());
        TEST_ASSERT(router.match(good1).has_value(),
                    "match(/foo/x/x/123) must succeed", true, router.match(good1).has_value());
        TEST_ASSERT(router.match(good2).has_value(),
                    "match(/foo/x/bar/) must succeed", true, router.match(good2).has_value());
    }

    {
        RadixRouter router;
        router.addHandler({"GET"}, "/conf/x", nullptr, {});
        router.addHandler({"GET"}, "/conf/*", nullptr, {});

        auto lit  = makeRequest("GET", "/conf/x");
        auto wild = makeRequest("GET", "/conf/y");

        TEST_ASSERT(router.match(lit).has_value(),
                    "match(/conf/x) must succeed", true, router.match(lit).has_value());
        TEST_ASSERT(router.match(wild).has_value(),
                    "match(/conf/y) must succeed", true, router.match(wild).has_value());
    }

    {
        RadixRouter router;
        router.addHandler({"GET"}, "/ts/", nullptr, {});

        auto a = makeRequest("GET", "/ts/");
        auto b = makeRequest("GET", "/ts");

        TEST_ASSERT(router.match(a).has_value(),
                    "match(/ts/) must succeed", true, router.match(a).has_value());
        TEST_ASSERT(!router.match(b).has_value(),
                    "match(/ts) must fail", false, router.match(b).has_value());
    }

    {
        RadixRouter router;
        param_constraint digits{ R"([0-9]+)", "" };
        router.addHandler({"GET"}, "/ping",               nullptr, {});
        router.addHandler({"GET"}, "/deposit/update/{id}",nullptr, {{"id",&digits}});

        std::string dump = router.dump();
        std::string expect =
            "/\n"
            "├─deposit\n"
            "│  └─update\n"
            "│     └─:id([0-9]+) [#]\n"
            "└─ping [#]\n";

        TEST_ASSERT(dump == expect,
                    "dump() structure",
                    expect,
                    dump);
    }

    std::cout << "All RadixRouter tests passed\n";
    return 0;
}
