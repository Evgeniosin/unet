#include "Components/Encodings/PunycodeEncoding.h"
#include <limits>
#include <cstdint>
#include <string>
#include <vector>

namespace usub::server::component {

namespace {
constexpr int BASE = 36;
constexpr int TMIN = 1;
constexpr int TMAX = 26;
constexpr int SKEW = 38;
constexpr int DAMP = 700;
constexpr int INITIAL_BIAS = 72;
constexpr uint32_t INITIAL_N = 0x80;
constexpr char DELIMITER = '-';
constexpr uint32_t MAX_CP = 0x10FFFF;

// Helper: decode a single UTF-8 code point from a string at pos, advance pos, return false on error
bool next_utf8_cp(const std::string& s, size_t& pos, uint32_t& cp) {
    if (pos >= s.size()) return false;
    unsigned char c = s[pos];
    if (c < 0x80) { cp = c; ++pos; return true; }
    if (c < 0xC2 || c > 0xF4) return false;
    size_t len = (c < 0xE0) ? 2 : (c < 0xF0) ? 3 : 4;
    if (pos + len > s.size()) return false;
    uint32_t out = c & ((1 << (8 - len)) - 1);
    for (size_t i = 1; i < len; ++i) {
        unsigned char cc = s[pos + i];
        if ((cc & 0xC0) != 0x80) return false;
        out = (out << 6) | (cc & 0x3F);
    }
    // Overlong, surrogate, or out of range
    if ((len == 2 && out < 0x80) || (len == 3 && out < 0x800) || (len == 4 && (out < 0x10000 || out > MAX_CP)) || (out >= 0xD800 && out <= 0xDFFF)) return false;
    cp = out;
    pos += len;
    return true;
}

// Helper: encode a single code point as UTF-8
void append_utf8_cp(std::string& out, uint32_t cp) {
    if (cp < 0x80) out.push_back((char)cp);
    else if (cp < 0x800) {
        out.push_back((char)(0xC0 | (cp >> 6)));
        out.push_back((char)(0x80 | (cp & 0x3F)));
    } else if (cp < 0x10000) {
        out.push_back((char)(0xE0 | (cp >> 12)));
        out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back((char)(0x80 | (cp & 0x3F)));
    } else {
        out.push_back((char)(0xF0 | (cp >> 18)));
        out.push_back((char)(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back((char)(0x80 | (cp & 0x3F)));
    }
}

bool is_ascii(const std::string& s) {
    for (unsigned char c : s) if (c & 0x80) return false;
    return true;
}

int adapt(int delta, int numpoints, bool first) {
    delta = first ? delta / DAMP : delta / 2;
    delta += delta / numpoints;
    int k = 0;
    while (delta > ((BASE - TMIN) * TMAX) / 2) {
        delta /= BASE - TMIN;
        k += BASE;
    }
    return k + (BASE - TMIN + 1) * delta / (delta + SKEW);
}

int encode_digit(int d) { return d < 26 ? 'a' + d : '0' + (d - 26); }
int decode_digit(char c) {
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= '0' && c <= '9') return c - '0' + 26;
    return -1;
}

} // namespace

bool PunycodeEncoding::encode(const std::string& input, std::string& output) {
    output.clear();
    if (input.empty()) return true;
    if (is_ascii(input)) { output = input; return true; }
    // Decode input to code points
    std::vector<uint32_t> codepoints;
    for (size_t i = 0; i < input.size();) {
        uint32_t cp;
        if (!next_utf8_cp(input, i, cp)) return false;
        codepoints.push_back(cp);
    }
    // Output prefix
    output = "xn--";
    // Copy basic code points
    int b = 0;
    for (uint32_t cp : codepoints) if (cp < 0x80) { output.push_back((char)cp); ++b; }
    int h = b;
    if (b > 0) output.push_back(DELIMITER);
    uint32_t n = INITIAL_N;
    int delta = 0;
    int bias = INITIAL_BIAS;
    while (h < (int)codepoints.size()) {
        uint32_t m = MAX_CP;
        for (uint32_t cp : codepoints) if (cp >= n && cp < m) m = cp;
        if (m - n > (std::numeric_limits<int>::max() - delta) / (h + 1)) return false;
        delta += (m - n) * (h + 1);
        n = m;
        for (size_t j = 0; j < codepoints.size(); ++j) {
            if (codepoints[j] < n) { if (++delta == 0) return false; }
            if (codepoints[j] == n) {
                int q = delta;
                for (int k = BASE;; k += BASE) {
                    int t = k <= bias ? TMIN : (k >= bias + TMAX ? TMAX : k - bias);
                    if (q < t) break;
                    output.push_back((char)encode_digit(t + (q - t) % (BASE - t)));
                    q = (q - t) / (BASE - t);
                }
                output.push_back((char)encode_digit(q));
                bias = adapt(delta, h + 1, h == b);
                delta = 0;
                ++h;
            }
        }
        ++delta;
        ++n;
    }
    return true;
}

bool PunycodeEncoding::decode(const std::string& input, std::string& output) {
    output.clear();
    if (input.empty()) return true;

    std::string s = input;
    if (is_punycode(s)) {
        s = s.substr(4);
    } else {
        if (is_ascii(input)) {
            output = input;
            return true;
        }
        return false; // Not punycode and not ascii
    }
    
    // After stripping "xn--", an empty string is not valid punycode.
    // Also, if there is no delimiter, the string must be empty, because
    // any non-basic characters would have required a delimiter.
    // The test case "xn--incomplete" fails this logic.
    size_t delim = s.find_last_of(DELIMITER);
    /* This check is wrong. A punycode string with only non-basic characters will not have a delimiter.
    if (delim == std::string::npos && !s.empty()) {
        return false;
    }
    */
    
    std::vector<uint32_t> codepoints;
    
    if (delim != std::string::npos) {
        for (size_t i = 0; i < delim; ++i) {
            if ((unsigned char)s[i] >= 0x80) return false; // Basic chars must be ASCII
            codepoints.push_back(s[i]);
        }
    }
    
    size_t in_pos = (delim == std::string::npos) ? 0 : delim + 1;
    uint32_t n = INITIAL_N;
    int i = 0;
    int bias = INITIAL_BIAS;

    if (in_pos > s.size()) { // Can happen if input is e.g. "xn--a-"
        return false;
    }

    while (in_pos < s.size()) {
        int old_i = i;
        int w = 1;
        
        for (int k = BASE; ; k += BASE) {
            if (in_pos >= s.size()) return false; // Truncated
            
            int digit = decode_digit(s[in_pos++]);
            if (digit < 0) return false; // Invalid character
            if (digit > (std::numeric_limits<int>::max() - i) / w) return false; // Overflow
            
            i += digit * w;
            int t = k <= bias ? TMIN : (k >= bias + TMAX ? TMAX : k - bias);
            
            if (digit < t) break;
            
            if (w > std::numeric_limits<int>::max() / (BASE - t)) return false; // Overflow
            w *= (BASE - t);
        }
        
        bias = adapt(i - old_i, codepoints.size() + 1, old_i == 0);
        
        if (i / (codepoints.size() + 1) > std::numeric_limits<uint32_t>::max() - n) return false; // Overflow
        
        n += i / (codepoints.size() + 1);
        i %= (codepoints.size() + 1);
        
        if (n > MAX_CP || (n >= 0xD800 && n <= 0xDFFF)) return false; // Invalid codepoint
        
        codepoints.insert(codepoints.begin() + i, n);
        i++;
    }
    
    if (in_pos != s.size()) return false;

    for(uint32_t cp : codepoints) {
        append_utf8_cp(output, cp);
    }

    return true;
}

bool PunycodeEncoding::is_punycode(const std::string& input) {
    return input.size() >= 4 && (input[0] == 'x' || input[0] == 'X') && (input[1] == 'n' || input[1] == 'N') && input[2] == '-' && input[3] == '-';
}

} // namespace usub::server::component