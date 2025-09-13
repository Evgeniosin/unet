//
// Created by Kirill Zhukov on 15.11.2023.
//

#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include "utils/toml/toml.hpp"
#include <iostream>


namespace usub::server {
    namespace configuration {
        namespace error {
            class WrongConfig final : public std::exception {
            public:
                // Constructor (C++11 onward)
                explicit WrongConfig(std::string msg) : message(std::move(msg)) {}

                // Override the what() method from the base class
                [[nodiscard]] const char *what() const noexcept override;

            private:
                std::string message;
            };
        };// namespace error

        struct Certificate {
            Certificate(std::string &domain, std::string &key_file, std::string &cert_file);
            std::string domain;
            std::string key_file;
            std::string cert_file;

            friend std::ostream &operator<<(std::ostream &os, const Certificate &certificate);
        };

        struct ListenerConfig {
            std::string ip_addr = "0.0.0.0";
            int port = 8080;
            bool ssl = false;
            int timeout = 10;
            int ipv = 0;
            std::string key_file;
            std::string cert_file;

            std::string getKeyFilePath();
            std::string getPemFilePath();

        };
        
        class ConfigReader {
        public:
            ConfigReader() = default;
            explicit ConfigReader(const std::string &config_path);

            std::vector<Certificate> getCerts();

            int64_t getThreads();

            int getBacklog();

            int getProtocolVersion();

            bool isBalanced();

            bool getStatusError();

            int getTimeout();

            int getWSTimeout();

            toml::node_view<toml::node> getKey(const std::string &key);

            toml::parse_result &getResult();

            std::vector<ListenerConfig>& getListeners();

        private:
            toml::parse_result res;
            std::vector<Certificate> certs;
            std::vector<ListenerConfig> listeners_;
        };

    };// namespace configuration
};    // namespace usub::server


#endif//CONFIGREADER_H
