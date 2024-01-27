#include <iostream>
#include <thread>
#include "server/server.h"
#include "utils/globals.h"

std::string getFileExtension(const std::string &fileName) {
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos) {
        return fileName.substr(dotPos + 1);
    }
    return ""; // Return empty string if no extension found
}

int main() {
    std::signal(SIGINT, interruptSignalHandler);
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGTERM, interruptSignalHandler);
    
    usub::server::Server redirector("server_config/redirectConfig.toml");
    redirector.handle(usub::server::core::REQUEST_TYPE::GET, R"(/)",
                  [&](usub::server::protocols::http::HttpRequest &request, usub::server::protocols::http::HttpResponse &response) {
                      response.setStatus("301");
                      response.addHeader("Location", "https://example.com");
                  });
    redirector.start();

    usub::server::Server server("server_config/config.toml");
    server.handle(usub::server::core::REQUEST_TYPE::GET, R"(/)",
                  [&](usub::server::protocols::http::HttpRequest &request,
                      usub::server::protocols::http::HttpResponse &response) {
                      response.writeFile("index.html");
                      std::cout << request.url << '\n';
                      response.setStatus("200");
                      response.addHeader((char *) "Content-Type", (char *) "text/html; charset=utf-8");
                  });
    server.handle(usub::server::core::REQUEST_TYPE::GET, R"(^\/.*\.[a-zA-Z0-9]+$)",
                  [&](usub::server::protocols::http::HttpRequest &request,
                      usub::server::protocols::http::HttpResponse &response) {
                      std::string path =
                              server.config->getKey("doc_root").as_string()->get() + request.headers.at(":path");
                      std::string extension = "application/";
                      extension.append(getFileExtension(path) == "js" ? "javascript" : "css");
                      response.writeFile(path);
                      response.setStatus("200");
                      path.clear();
                  });
    server.start();
    while (!signalReceived.load()) {
	
    }

    return 0;
}
