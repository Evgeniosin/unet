/* C++ code produced by gperf version 3.0.1 */
/* Command-line: gperf -C -t -L C++ -Z HTTPHeaderLookup headers.gperf  */
/* Computed positions: -k'1,3,11,14,22,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "headers.gperf"

#include "Protocols/HTTP/header_lookup.h"

#line 11 "headers.gperf"
#define TOTAL_KEYWORDS 144
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 38
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 420
/* maximum key range = 419, duplicates = 0 */

inline unsigned int
HTTPHeaderLookup::hash(const char *str, unsigned int len) {
    static const unsigned short asso_values[] =
            {
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 30, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 45, 5, 0,
                    60, 0, 120, 75, 45, 5, 421, 90, 0, 145,
                    20, 80, 75, 421, 5, 0, 0, 160, 110, 150,
                    75, 55, 0, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421, 421, 421, 421, 421,
                    421, 421, 421, 421, 421, 421};
    int hval = len;

    switch (hval) {
        default:
            hval += asso_values[std::tolower((unsigned char) str[21])];
        /*FALLTHROUGH*/
        case 21:
        case 20:
        case 19:
        case 18:
        case 17:
        case 16:
        case 15:
        case 14:
            hval += asso_values[std::tolower((unsigned char) str[13])];
        /*FALLTHROUGH*/
        case 13:
        case 12:
        case 11:
            hval += asso_values[std::tolower((unsigned char) str[10])];
        /*FALLTHROUGH*/
        case 10:
        case 9:
        case 8:
        case 7:
        case 6:
        case 5:
        case 4:
        case 3:
            hval += asso_values[std::tolower((unsigned char) str[2])];
        /*FALLTHROUGH*/
        case 2:
        case 1:
            hval += asso_values[std::tolower((unsigned char) str[0])];
            break;
    }
    return hval + asso_values[std::tolower((unsigned char) str[len - 1])];
}

const struct HeaderInfo *
HTTPHeaderLookup::lookupHeader(const char *str, unsigned int len) {
    static const struct HeaderInfo wordlist[] =
            {
                    {""},
                    {""},
#line 138 "headers.gperf"
                    {"te", usub::server::component::HeaderEnum::TE},
#line 65 "headers.gperf"
                    {"ect", usub::server::component::HeaderEnum::ECT},
                    {""},
                    {""},
                    {""},
#line 120 "headers.gperf"
                    {"sec-gpc", usub::server::component::HeaderEnum::Sec_GPC},
#line 101 "headers.gperf"
                    {"rtt", usub::server::component::HeaderEnum::RTT},
                    {""},
#line 132 "headers.gperf"
                    {"set-cookie", usub::server::component::HeaderEnum::Set_Cookie},
#line 121 "headers.gperf"
                    {"sec-purpose", usub::server::component::HeaderEnum::Sec_Purpose},
                    {""},
                    {""},
#line 118 "headers.gperf"
                    {"sec-fetch-site", usub::server::component::HeaderEnum::Sec_Fetch_Site},
                    {""},
#line 127 "headers.gperf"
                    {"server", usub::server::component::HeaderEnum::Server},
                    {""},
#line 40 "headers.gperf"
                    {"cache-control", usub::server::component::HeaderEnum::Cache_Control},
                    {""},
                    {""},
                    {""},
                    {""},
#line 83 "headers.gperf"
                    {"nel", usub::server::component::HeaderEnum::NEL},
                    {""},
                    {""},
#line 100 "headers.gperf"
                    {"retry-after", usub::server::component::HeaderEnum::Retry_After},
                    {""},
#line 81 "headers.gperf"
                    {"location", usub::server::component::HeaderEnum::Location},
#line 133 "headers.gperf"
                    {"set-login", usub::server::component::HeaderEnum::Set_Login},
#line 93 "headers.gperf"
                    {"range", usub::server::component::HeaderEnum::Range},
                    {""},
                    {""},
                    {""},
#line 129 "headers.gperf"
                    {"service-worker", usub::server::component::HeaderEnum::Service_Worker},
                    {""},
                    {""},
                    {""},
                    {""},
#line 103 "headers.gperf"
                    {"sec-browsing-topics", usub::server::component::HeaderEnum::Sec_Browsing_Topics},
                    {""},
#line 45 "headers.gperf"
                    {"content-dpr", usub::server::component::HeaderEnum::Content_DPR},
#line 109 "headers.gperf"
                    {"sec-ch-ua-bitness", usub::server::component::HeaderEnum::Sec_CH_UA_Bitness},
#line 76 "headers.gperf"
                    {"if-range", usub::server::component::HeaderEnum::If_Range},
                    {""},
                    {""},
                    {""},
                    {""},
#line 32 "headers.gperf"
                    {"age", usub::server::component::HeaderEnum::Age},
#line 72 "headers.gperf"
                    {"host", usub::server::component::HeaderEnum::Host},
#line 42 "headers.gperf"
                    {"connection", usub::server::component::HeaderEnum::Connection},
#line 17 "headers.gperf"
                    {"accept", usub::server::component::HeaderEnum::Accept},
#line 34 "headers.gperf"
                    {"alt-svc", usub::server::component::HeaderEnum::Alt_Svc},
#line 50 "headers.gperf"
                    {"content-range", usub::server::component::HeaderEnum::Content_Range},
#line 107 "headers.gperf"
                    {"sec-ch-ua", usub::server::component::HeaderEnum::Sec_CH_UA},
                    {""},
#line 22 "headers.gperf"
                    {"accept-post", usub::server::component::HeaderEnum::Accept_Post},
#line 140 "headers.gperf"
                    {"trailer", usub::server::component::HeaderEnum::Trailer},
                    {""},
#line 44 "headers.gperf"
                    {"content-disposition", usub::server::component::HeaderEnum::Content_Disposition},
#line 64 "headers.gperf"
                    {"early-data", usub::server::component::HeaderEnum::Early_Data},
#line 49 "headers.gperf"
                    {"content-location", usub::server::component::HeaderEnum::Content_Location},
                    {""},
#line 61 "headers.gperf"
                    {"dnt", usub::server::component::HeaderEnum::DNT},
#line 59 "headers.gperf"
                    {"date", usub::server::component::HeaderEnum::Date},
                    {""},
                    {""},
#line 29 "headers.gperf"
                    {"access-control-max-age", usub::server::component::HeaderEnum::Access_Control_Max_Age},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
#line 63 "headers.gperf"
                    {"dpr", usub::server::component::HeaderEnum::DPR},
#line 116 "headers.gperf"
                    {"sec-fetch-dest", usub::server::component::HeaderEnum::Sec_Fetch_Dest},
#line 30 "headers.gperf"
                    {"access-control-request-headers", usub::server::component::HeaderEnum::Access_Control_Request_Headers},
                    {""},
#line 24 "headers.gperf"
                    {"access-control-allow-credentials", usub::server::component::HeaderEnum::Access_Control_Allow_Credentials},
#line 79 "headers.gperf"
                    {"last-modified", usub::server::component::HeaderEnum::Last_Modified},
                    {""},
                    {""},
#line 67 "headers.gperf"
                    {"expect", usub::server::component::HeaderEnum::Expect},
#line 69 "headers.gperf"
                    {"expires", usub::server::component::HeaderEnum::Expires},
#line 39 "headers.gperf"
                    {"authorization", usub::server::component::HeaderEnum::Authorization},
#line 68 "headers.gperf"
                    {"expect-ct", usub::server::component::HeaderEnum::Expect_CT},
                    {""},
#line 54 "headers.gperf"
                    {"cookie", usub::server::component::HeaderEnum::Cookie},
                    {""},
#line 73 "headers.gperf"
                    {"if-match", usub::server::component::HeaderEnum::If_Match},
#line 77 "headers.gperf"
                    {"if-unmodified-since", usub::server::component::HeaderEnum::If_Unmodified_Since},
#line 41 "headers.gperf"
                    {"clear-site-data", usub::server::component::HeaderEnum::Clear_Site_Data},
#line 99 "headers.gperf"
                    {"repr-digest", usub::server::component::HeaderEnum::Repr_Digest},
#line 139 "headers.gperf"
                    {"tk", usub::server::component::HeaderEnum::Tk},
#line 75 "headers.gperf"
                    {"if-none-match", usub::server::component::HeaderEnum::If_None_Match},
                    {""},
                    {""},
                    {""},
#line 151 "headers.gperf"
                    {"x-content-type-options", usub::server::component::HeaderEnum::X_Content_Type_Options},
#line 128 "headers.gperf"
                    {"server-timing", usub::server::component::HeaderEnum::Server_Timing},
#line 18 "headers.gperf"
                    {"accept-ch", usub::server::component::HeaderEnum::Accept_CH},
#line 78 "headers.gperf"
                    {"keep-alive", usub::server::component::HeaderEnum::Keep_Alive},
#line 47 "headers.gperf"
                    {"content-language", usub::server::component::HeaderEnum::Content_Language},
#line 21 "headers.gperf"
                    {"accept-patch", usub::server::component::HeaderEnum::Accept_Patch},
#line 51 "headers.gperf"
                    {"content-security-policy", usub::server::component::HeaderEnum::Content_Security_Policy},
#line 28 "headers.gperf"
                    {"access-control-expose-headers", usub::server::component::HeaderEnum::Access_Control_Expose_Headers},
                    {""},
#line 55 "headers.gperf"
                    {"critical-ch", usub::server::component::HeaderEnum::Critical_CH},
#line 53 "headers.gperf"
                    {"content-type", usub::server::component::HeaderEnum::Content_Type},
#line 131 "headers.gperf"
                    {"service-worker-navigation-preload", usub::server::component::HeaderEnum::Service_Worker_Navigation_Preload},
#line 43 "headers.gperf"
                    {"content-digest", usub::server::component::HeaderEnum::Content_Digest},
                    {""},
#line 86 "headers.gperf"
                    {"origin", usub::server::component::HeaderEnum::Origin},
                    {""},
#line 35 "headers.gperf"
                    {"alt-used", usub::server::component::HeaderEnum::Alt_Used},
#line 80 "headers.gperf"
                    {"link", usub::server::component::HeaderEnum::Link},
#line 52 "headers.gperf"
                    {"content-security-policy-report-only", usub::server::component::HeaderEnum::Content_Security_Policy_Report_Only},
#line 46 "headers.gperf"
                    {"content-encoding", usub::server::component::HeaderEnum::Content_Encoding},
#line 74 "headers.gperf"
                    {"if-modified-since", usub::server::component::HeaderEnum::If_Modified_Since},
#line 25 "headers.gperf"
                    {"access-control-allow-headers", usub::server::component::HeaderEnum::Access_Control_Allow_Headers},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
#line 66 "headers.gperf"
                    {"etag", usub::server::component::HeaderEnum::Etag},
#line 36 "headers.gperf"
                    {"attribution-reporting-eligible", usub::server::component::HeaderEnum::Attribution_Reporting_Eligible},
#line 115 "headers.gperf"
                    {"sec-ch-ua-platform-version", usub::server::component::HeaderEnum::Sec_CH_UA_Platform_Version},
                    {""},
                    {""},
                    {""},
#line 87 "headers.gperf"
                    {"origin-agent-cluster", usub::server::component::HeaderEnum::Origin_Agent_Cluster},
                    {""},
#line 37 "headers.gperf"
                    {"attribution-reporting-register-source", usub::server::component::HeaderEnum::Attribution_Reporting_Register_Source},
#line 23 "headers.gperf"
                    {"accept-ranges", usub::server::component::HeaderEnum::Accept_Ranges},
#line 31 "headers.gperf"
                    {"access-control-request-method", usub::server::component::HeaderEnum::Access_Control_Request_Method},
                    {""},
                    {""},
#line 94 "headers.gperf"
                    {"referer", usub::server::component::HeaderEnum::Referer},
#line 38 "headers.gperf"
                    {"attribution-reporting-register-trigger", usub::server::component::HeaderEnum::Attribution_Reporting_Register_Trigger},
                    {""},
#line 122 "headers.gperf"
                    {"sec-websocket-accept", usub::server::component::HeaderEnum::Sec_WebSocket_Accept},
                    {""},
#line 125 "headers.gperf"
                    {"sec-websocket-protocol", usub::server::component::HeaderEnum::Sec_WebSocket_Protocol},
#line 90 "headers.gperf"
                    {"priority", usub::server::component::HeaderEnum::Priority},
#line 48 "headers.gperf"
                    {"content-length", usub::server::component::HeaderEnum::Content_Length},
                    {""},
                    {""},
#line 104 "headers.gperf"
                    {"sec-ch-prefers-color-scheme", usub::server::component::HeaderEnum::Sec_CH_Prefers_Color_Scheme},
                    {""},
#line 108 "headers.gperf"
                    {"sec-ch-ua-arch", usub::server::component::HeaderEnum::Sec_CH_UA_Arch},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
#line 130 "headers.gperf"
                    {"service-worker-allowed", usub::server::component::HeaderEnum::Service_Worker_Allowed},
                    {""},
#line 117 "headers.gperf"
                    {"sec-fetch-mode", usub::server::component::HeaderEnum::Sec_Fetch_Mode},
#line 113 "headers.gperf"
                    {"sec-ch-ua-model", usub::server::component::HeaderEnum::Sec_CH_UA_Model},
#line 126 "headers.gperf"
                    {"sec-websocket-version", usub::server::component::HeaderEnum::Sec_WebSocket_Version},
                    {""},
                    {""},
#line 102 "headers.gperf"
                    {"save-data", usub::server::component::HeaderEnum::Save_Data},
                    {""},
#line 112 "headers.gperf"
                    {"sec-ch-ua-mobile", usub::server::component::HeaderEnum::Sec_CH_UA_Mobile},
#line 111 "headers.gperf"
                    {"sec-ch-ua-full-version-list", usub::server::component::HeaderEnum::Sec_CH_UA_Full_Version_List},
                    {""},
#line 97 "headers.gperf"
                    {"report-to", usub::server::component::HeaderEnum::Report_To},
#line 144 "headers.gperf"
                    {"user-agent", usub::server::component::HeaderEnum::User_Agent},
#line 89 "headers.gperf"
                    {"pragma", usub::server::component::HeaderEnum::Pragma},
#line 27 "headers.gperf"
                    {"access-control-allow-origin", usub::server::component::HeaderEnum::Access_Control_Allow_Origin},
                    {""},
#line 98 "headers.gperf"
                    {"reporting-endpoints", usub::server::component::HeaderEnum::Reporting_Endpoints},
                    {""},
                    {""},
#line 96 "headers.gperf"
                    {"refresh", usub::server::component::HeaderEnum::Refresh},
#line 91 "headers.gperf"
                    {"proxy-authenticate", usub::server::component::HeaderEnum::Proxy_Authenticate},
                    {""},
                    {""},
#line 137 "headers.gperf"
                    {"supports-loading-mode", usub::server::component::HeaderEnum::Supports_Loading_Mode},
#line 110 "headers.gperf"
                    {"sec-ch-ua-full-version", usub::server::component::HeaderEnum::Sec_CH_UA_Full_Version},
                    {""},
#line 119 "headers.gperf"
                    {"sec-fetch-user", usub::server::component::HeaderEnum::Sec_Fetch_User},
                    {""},
                    {""},
                    {""},
#line 85 "headers.gperf"
                    {"observe-browsing-topics", usub::server::component::HeaderEnum::Observe_Browsing_Topics},
                    {""},
#line 136 "headers.gperf"
                    {"strict-transport-security", usub::server::component::HeaderEnum::Strict_Transport_Security},
#line 160 "headers.gperf"
                    {"x-xss-protection", usub::server::component::HeaderEnum::X_XSS_Protection},
#line 124 "headers.gperf"
                    {"sec-websocket-key", usub::server::component::HeaderEnum::Sec_WebSocket_Key},
                    {""},
#line 70 "headers.gperf"
                    {"forwarded", usub::server::component::HeaderEnum::Forwarded},
                    {""},
                    {""},
#line 135 "headers.gperf"
                    {"speculation-rules", usub::server::component::HeaderEnum::Speculation_Rules},
#line 56 "headers.gperf"
                    {"cross-origin-embedder-policy", usub::server::component::HeaderEnum::Cross_Origin_Embedder_Policy},
#line 84 "headers.gperf"
                    {"no-vary-search", usub::server::component::HeaderEnum::No_Vary_Search},
#line 33 "headers.gperf"
                    {"allow", usub::server::component::HeaderEnum::Allow},
                    {""},
#line 152 "headers.gperf"
                    {"x-dns-prefetch-control", usub::server::component::HeaderEnum::X_DNS_Prefetch_Control},
#line 58 "headers.gperf"
                    {"cross-origin-resource-policy", usub::server::component::HeaderEnum::Cross_Origin_Resource_Policy},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
#line 20 "headers.gperf"
                    {"accept-language", usub::server::component::HeaderEnum::Accept_Language},
                    {""},
#line 159 "headers.gperf"
                    {"x-robots-tag", usub::server::component::HeaderEnum::X_Robots_Tag},
                    {""},
                    {""},
                    {""},
                    {""},
#line 141 "headers.gperf"
                    {"transfer-encoding", usub::server::component::HeaderEnum::Transfer_Encoding},
#line 26 "headers.gperf"
                    {"access-control-allow-methods", usub::server::component::HeaderEnum::Access_Control_Allow_Methods},
#line 145 "headers.gperf"
                    {"viewport-width", usub::server::component::HeaderEnum::Viewport_Width},
                    {""},
                    {""},
#line 158 "headers.gperf"
                    {"x-powered-by", usub::server::component::HeaderEnum::X_Powered_By},
                    {""},
#line 123 "headers.gperf"
                    {"sec-websocket-extensions", usub::server::component::HeaderEnum::Sec_WebSocket_Extensions},
                    {""},
                    {""},
                    {""},
                    {""},
#line 105 "headers.gperf"
                    {"sec-ch-prefers-reduced-motion", usub::server::component::HeaderEnum::Sec_CH_Prefers_Reduced_Motion},
#line 156 "headers.gperf"
                    {"x-frame-options", usub::server::component::HeaderEnum::X_Frame_Options},
                    {""},
                    {""},
#line 88 "headers.gperf"
                    {"permissions-policy", usub::server::component::HeaderEnum::Permissions_Policy},
                    {""},
#line 19 "headers.gperf"
                    {"accept-encoding", usub::server::component::HeaderEnum::Accept_Encoding},
                    {""},
#line 148 "headers.gperf"
                    {"warning", usub::server::component::HeaderEnum::Warning},
#line 114 "headers.gperf"
                    {"sec-ch-ua-platform", usub::server::component::HeaderEnum::Sec_CH_UA_Platform},
                    {""},
                    {""},
                    {""},
#line 142 "headers.gperf"
                    {"upgrade", usub::server::component::HeaderEnum::Upgrade},
                    {""},
#line 134 "headers.gperf"
                    {"sourcemap", usub::server::component::HeaderEnum::SourceMap},
                    {""},
#line 147 "headers.gperf"
                    {"want-repr-digest", usub::server::component::HeaderEnum::Want_Repr_Digest},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
#line 149 "headers.gperf"
                    {"width", usub::server::component::HeaderEnum::Width},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
#line 146 "headers.gperf"
                    {"want-content-digest", usub::server::component::HeaderEnum::Want_Content_Digest},
#line 106 "headers.gperf"
                    {"sec-ch-prefers-reduced-transparency", usub::server::component::HeaderEnum::Sec_CH_Prefers_Reduced_Transparency},
                    {""},
                    {""},
                    {""},
#line 92 "headers.gperf"
                    {"proxy-authorization", usub::server::component::HeaderEnum::Proxy_Authorization},
#line 95 "headers.gperf"
                    {"referrer-policy", usub::server::component::HeaderEnum::Referrer_Policy},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
#line 82 "headers.gperf"
                    {"max-forwards", usub::server::component::HeaderEnum::Max_Forwards},
#line 157 "headers.gperf"
                    {"x-permitted-cross-domain-policies", usub::server::component::HeaderEnum::X_Permitted_Cross_Domain_Policies},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
#line 62 "headers.gperf"
                    {"downlink", usub::server::component::HeaderEnum::Downlink},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
#line 60 "headers.gperf"
                    {"device-memory", usub::server::component::HeaderEnum::Device_Memory},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
#line 57 "headers.gperf"
                    {"cross-origin-opener-policy", usub::server::component::HeaderEnum::Cross_Origin_Opener_Policy},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
#line 71 "headers.gperf"
                    {"from", usub::server::component::HeaderEnum::From},
                    {""},
#line 154 "headers.gperf"
                    {"x-forwarded-host", usub::server::component::HeaderEnum::X_Forwarded_Host},
                    {""},
                    {""},
                    {""},
#line 153 "headers.gperf"
                    {"x-forwarded-for", usub::server::component::HeaderEnum::X_Forwarded_For},
                    {""},
#line 155 "headers.gperf"
                    {"x-forwarded-proto", usub::server::component::HeaderEnum::X_Forwarded_Proto},
                    {""},
                    {""},
                    {""},
#line 150 "headers.gperf"
                    {"www-authenticate", usub::server::component::HeaderEnum::WWW_Authenticate},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
                    {""},
#line 143 "headers.gperf"
                    {"upgrade-insecure-requests", usub::server::component::HeaderEnum::Upgrade_Insecure_Requests}};

    if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
        int key = hash(str, len);

        if (key <= MAX_HASH_VALUE && key >= 0) {
            const char *s = wordlist[key].name;

            if (/**str == *s && !strcmp(str + 1, s + 1)*/ usub::utils::case_insensitive_equal(str, s, len)) {
                return &wordlist[key];
            }
        }
        return 0;
    }
    return 0;
}
