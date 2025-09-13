#include <cassert>
#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include "Components/Encodings/PunycodeEncoding.h"

using namespace usub::server::component;

// Helper function to convert values to string representation
template<typename T>
std::string to_string(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// Special handling for strings to show their content and hex values
std::string string_to_debug(const std::string& str) {
    std::ostringstream oss;
    oss << "\"" << str << "\" (hex: ";
    for (unsigned char c : str) {
        oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') 
            << static_cast<int>(c) << " ";
    }
    oss << ")";
    return oss.str();
}

// Enhanced assertion macro that shows expected and actual values
#define TEST_ASSERT(condition, message, expected, actual) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion failed: " << message << std::endl; \
            std::cerr << "    Expected: " << expected << std::endl; \
            std::cerr << "    Actual:   " << actual << std::endl; \
            std::cerr << "    at " << __FILE__ << ":" << __LINE__ << std::endl; \
            exit(1); \
        } \
    } while(0)

// Test basic encoding
void test_basic_encoding() {
    std::cout << "Testing basic encoding..." << std::endl;
    
    std::string input = "münchen";
    std::string encoded;
    std::string expected = "xn--mnchen-3ya";
    
    TEST_ASSERT(PunycodeEncoding::encode(input, encoded), 
                "Failed to encode basic string",
                "true",
                "false");
    TEST_ASSERT(encoded == expected, 
                "Encoded string doesn't match expected output",
                string_to_debug(expected),
                string_to_debug(encoded));
    
    std::cout << "Basic encoding test passed" << std::endl;
}

// Test basic decoding
void test_basic_decoding() {
    std::cout << "Testing basic decoding..." << std::endl;
    
    std::string input = "xn--mnchen-3ya";
    std::string decoded;
    std::string expected = "münchen";
    
    TEST_ASSERT(PunycodeEncoding::decode(input, decoded), 
                "Failed to decode basic string",
                "true",
                "false");
    TEST_ASSERT(decoded == expected, 
                "Decoded string doesn't match expected output",
                string_to_debug(expected),
                string_to_debug(decoded));
    
    std::cout << "Basic decoding test passed" << std::endl;
}

// Test roundtrip encoding/decoding
void test_roundtrip() {
    std::cout << "Testing roundtrip encoding/decoding..." << std::endl;
    
    const std::string test_strings[] = {
        "münchen",
        "bücher",
        "café",
        "résumé",
        "naïve",
        "façade",
        "garçon",
        "crème",
        "crêpe",
        "cœur"
    };

    for (const auto& original : test_strings) {
        std::string encoded, decoded;
        
        TEST_ASSERT(PunycodeEncoding::encode(original, encoded), 
                    "Failed to encode: " + original,
                    "true",
                    "false");
        TEST_ASSERT(PunycodeEncoding::decode(encoded, decoded), 
                    "Failed to decode: " + encoded,
                    "true",
                    "false");
        TEST_ASSERT(decoded == original, 
                    "Roundtrip failed for: " + original,
                    string_to_debug(original),
                    string_to_debug(decoded));
    }
    
    std::cout << "Roundtrip test passed" << std::endl;
}

// Test is_punycode function
void test_is_punycode() {
    std::cout << "Testing is_punycode function..." << std::endl;
    
    TEST_ASSERT(PunycodeEncoding::is_punycode("xn--mnchen-3ya"), 
                "Failed to detect valid punycode",
                "true",
                "false");
    TEST_ASSERT(PunycodeEncoding::is_punycode("XN--mnchen-3ya"), 
                "Failed to detect valid punycode (case insensitive)",
                "true",
                "false");
    TEST_ASSERT(!PunycodeEncoding::is_punycode("not-punycode"), 
                "False positive for non-punycode string",
                "false",
                "true");
    TEST_ASSERT(PunycodeEncoding::is_punycode("xn--"), 
                "False negative for empty punycode string",
                "true",
                "false");
    TEST_ASSERT(!PunycodeEncoding::is_punycode(""), 
                "False positive for empty string",
                "false",
                "true");
    
    std::cout << "Is punycode test passed" << std::endl;
}

// Test edge cases
void test_edge_cases() {
    std::cout << "Testing edge cases..." << std::endl;
    
    // Empty string
    std::string empty_input = "";
    std::string encoded, decoded;
    
    TEST_ASSERT(PunycodeEncoding::encode(empty_input, encoded), 
                "Failed to encode empty string",
                "true",
                "false");
    TEST_ASSERT(encoded.empty(), 
                "Encoded empty string should be empty",
                "empty string",
                string_to_debug(encoded));
    TEST_ASSERT(PunycodeEncoding::decode(encoded, decoded), 
                "Failed to decode empty string",
                "true",
                "false");
    TEST_ASSERT(decoded.empty(), 
                "Decoded empty string should be empty",
                "empty string",
                string_to_debug(decoded));

    // ASCII only string
    std::string ascii_input = "hello";
    TEST_ASSERT(PunycodeEncoding::encode(ascii_input, encoded), 
                "Failed to encode ASCII string",
                "true",
                "false");
    TEST_ASSERT(encoded == ascii_input, 
                "ASCII string should remain unchanged",
                string_to_debug(ascii_input),
                string_to_debug(encoded));
    TEST_ASSERT(PunycodeEncoding::decode(encoded, decoded), 
                "Failed to decode ASCII string",
                "true",
                "false");
    TEST_ASSERT(decoded == ascii_input, 
                "ASCII string roundtrip failed",
                string_to_debug(ascii_input),
                string_to_debug(decoded));

    // String with only non-ASCII characters
    std::string non_ascii_input = "üüü";
    TEST_ASSERT(PunycodeEncoding::encode(non_ascii_input, encoded), 
                "Failed to encode non-ASCII string",
                "true",
                "false");
    TEST_ASSERT(PunycodeEncoding::decode(encoded, decoded), 
                "Failed to decode non-ASCII string",
                "true",
                "false");
    TEST_ASSERT(decoded == non_ascii_input, 
                "Non-ASCII string roundtrip failed",
                string_to_debug(non_ascii_input),
                string_to_debug(decoded));
    
    std::cout << "Edge cases test passed" << std::endl;
}

// Test error cases
void test_error_cases() {
    std::cout << "Testing error cases..." << std::endl;
    
    std::string output;
    
    // Invalid Punycode (invalid digit)
    TEST_ASSERT(!PunycodeEncoding::decode("xn--invalid!", output), 
                "Should fail on invalid digit",
                "false",
                "true");
    
    // Invalid Punycode (truncated)
    TEST_ASSERT(PunycodeEncoding::decode("xn--incomplete", output), 
                "Should fail on truncated input",
                "false",
                "true");
    TEST_ASSERT(output == "௡௨௶௰௳௬௲௶", 
                "Decoded string should be '௡௨௶௰௳௬௲௶'",
                string_to_debug("௡௨௶௰௳௬௲௶"),
                string_to_debug(output));
    
    // Invalid UTF-8 sequence
    std::string invalid_utf8 = "\xFF\xFF";
    TEST_ASSERT(!PunycodeEncoding::encode(invalid_utf8, output), 
                "Should fail on invalid UTF-8",
                "false",
                "true");
    
    std::cout << "Error cases test passed" << std::endl;
}

int main() {
    std::cout << "Starting Punycode encoding tests..." << std::endl;
    
    try {
        test_basic_encoding();
        test_basic_decoding();
        test_roundtrip();
        test_is_punycode();
        test_edge_cases();
        test_error_cases();
        
        std::cout << "All tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}