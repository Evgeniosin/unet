#ifndef HEADERS_LOOKUP_H
#define HEADERS_LOOKUP_H

#include "HeaderEnum.h"
#include <cstring>

#include "utils/string_utils.h"

struct HeaderInfo {
    const char *name;
    usub::server::component::HeaderEnum id;
};

class HTTPHeaderLookup {
public:
    static const HeaderInfo *lookupHeader(const char *str, unsigned int len);

private:
    static unsigned int hash(const char *str, unsigned int len);
};

#endif// HEADERS_LOOKUP_H
