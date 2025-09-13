//
// Created by Kirill Zhukov on 15.11.2023.
//

#include "utils/configuration/ConfigReader.h"

// #include "logging/logger.h"

namespace usub::server::configuration {

    usub::server::configuration::ConfigReader::ConfigReader(const std::string &config_path) {
        try {
            this->res = toml::parse_file(config_path);
        } catch (toml::v3::ex::parse_error &e) {
            std::cerr << "`ConfigReader` constructor error: << " << e.what() << std::endl;
            exit(9);
        }
        if (res.contains("listener") && res.get_as<toml::array>("listener")) {
            for (const auto& listener : *res.get_as<toml::array>("listener")) {
                if (listener.is_table()) {
                    ListenerConfig config;
                    const auto& table = *listener.as_table();

                    if (table.contains("ip_addr"))
                        config.ip_addr = table["ip_addr"].as_string()->get();
                    if (table.contains("port"))
                        config.port = table["port"].as_integer()->get();
                    if (table.contains("ssl"))
                        config.ssl = table["ssl"].as_boolean()->get();
                    if (table.contains("timeout"))
                        config.timeout = table["timeout"].as_integer()->get();
                    if (table.contains("ipv"))
                        config.ipv = table["ipv"].as_integer()->get();
                    if (table.contains("key_file"))
                        config.key_file = table["key_file"].as_string()->get();
                    if (table.contains("cert_file"))
                        config.cert_file = table["cert_file"].as_string()->get();
                    
                    listeners_.push_back(config);
                }
            }
        } else {
            listeners_.emplace_back();
        }
    }

    toml::node_view<toml::node> usub::server::configuration::ConfigReader::getKey(const std::string &key) {
        return this->res[key];
    }

    toml::parse_result &usub::server::configuration::ConfigReader::getResult() {
        return this->res;
    }

    int64_t usub::server::configuration::ConfigReader::getThreads() {
        if (this->res.contains("server") || this->res.get_as<toml::table>("server")->contains("threads")) {
            return this->res["server"]["threads"].as_integer()->get();
        }
        return 1;
    }

    int usub::server::configuration::ConfigReader::getBacklog() {
        if (!this->res.contains("server") || !this->res.get_as<toml::table>("server")->contains("backlog")) {
            return 50;
        }
        return this->res["server"]["backlog"].as_integer()->get();
    }

    int usub::server::configuration::ConfigReader::getProtocolVersion() {
        if (this->res.contains("server")) {
            if (this->res.get_as<toml::table>("server")->contains("http_version")) {
                return this->res["server"]["http_version"].as_integer()->get();
            }
            if (this->res.get_as<toml::table>("server")->contains("ssl")) {
                if (this->res["server"]["ssl"].as_boolean()->get())
                    return 2;
                return 1;
            }
        }
        return 2;
    }

    bool usub::server::configuration::ConfigReader::getStatusError() {
        if (this->res.contains("log") && this->res.get_as<toml::table>("log")->contains("status_error")) {
            return this->res["log"]["status_error"].as_boolean()->get();
        }
        return true;
    }

    int usub::server::configuration::ConfigReader::getTimeout() {
        if (this->res.contains("server") && this->res.get_as<toml::table>("server")->contains("timeout")) {
            return this->res["server"]["timeout"].as_integer()->get();
        }
        return 10;
    }

    int usub::server::configuration::ConfigReader::getWSTimeout() {
        if (this->res.contains("server") && this->res.get_as<toml::table>("server")->contains("ws_timeout")) {
            return this->res["server"]["ws_timeout"].as_integer()->get();
        }
        return 20000;
    }

    std::vector<usub::server::configuration::Certificate> usub::server::configuration::ConfigReader::getCerts() {
        if (!this->res.contains("certs")) throw error::WrongConfig("No certificates were provided");
        for (const auto &cert_node: *res["certs"].as_array()) {
            const auto &cert = *cert_node.as_table();
            std::string domain = cert["domain"].value_or("");
            std::string key_file = cert["key_file"].value_or("");
            std::string cert_file = cert["cert_file"].value_or("");
            this->certs.emplace_back(domain, key_file, cert_file);
        }
        return this->certs;
    }

    bool ConfigReader::isBalanced() {
        if (this->res.contains("server") && this->res.get_as<toml::table>("server")->contains("balanced")) return this->res["server"]["balanced"].as_boolean();
        return false;
    }

    usub::server::configuration::Certificate::Certificate(std::string &domain, std::string &key_file,
                                                          std::string &cert_file) : domain(domain), key_file(key_file),
                                                                                    cert_file(cert_file) {}

    std::ostream &operator<<(std::ostream &os, const Certificate &certificate) {
        os << "domain: " << certificate.domain << " key_file: " << certificate.key_file << " cert_file: "
           << certificate.cert_file;
        return os;
    }

    const char *error::WrongConfig::what() const noexcept {
        return this->message.c_str();
    }

    std::vector<ListenerConfig>& usub::server::configuration::ConfigReader::getListeners() {
        return listeners_;
    }
}// namespace usub::server::configuration
