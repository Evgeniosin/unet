#include "server/server.h"
#include <cstdio>
#include <iostream>
#include <numeric>
#include <thread>

// #include "Coroutines/Coroutine.h"
#include "Protocols/HTTP/Message.h"
// #include "utils/globals.h"

std::optional<std::string> getMultiHeader(std::string &&headerName, const std::unordered_multimap<std::string, std::string> &headers) {
    auto it = headers.find(headerName);
    if (it != headers.end())
        return it->second;
    else
        return std::nullopt;
}

std::string getFileExtension(const std::string &fileName) {
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos) {
        return fileName.substr(dotPos + 1);
    }
    return "";// Return empty string if no extension found
}

uint64_t request_cntr{};

bool headerMiddle(const usub::server::protocols::http::Request &request, usub::server::protocols::http::Response &response) {
    std::cout << "header middleware reached" << std::endl;
    return true;
}

bool globalMiddle(const usub::server::protocols::http::Request &request, usub::server::protocols::http::Response &response) {
    request_cntr++;
    //    std::cout<<"global middleware reached"<<std::endl;
    return true;
}


bool bodyMiddle(const usub::server::protocols::http::Request &request, usub::server::protocols::http::Response &response) {
    //    std::cout<<"body middleware reached"<<std::endl;
    return true;
}

bool responseMiddle(const usub::server::protocols::http::Request &request, usub::server::protocols::http::Response &response) {
    // std::cout << "request middleware reached " << request_cntr << std::endl;
    return true;
}

void handlerFunction(usub::server::protocols::http::Request &request, usub::server::protocols::http::Response &response) {
    std::string res = "Endpoint DATA: " + request.getBody() + "\nWith uri params numeric: " +
                      request.uri_params["numeric"] + "\nAnd word: " + request.uri_params["word"] +
                      "\nAnd anything: " + request.uri_params["anything"] + "\nURI is: " + request.getFullURL() + "\n";
    response.setStatus(200).setMessage("OK").addHeader("Content-Type", "text/html").setBody("Hello World! How are you");
    // std::cout << res << std::endl;
    return;
}
#include "../include/Components/Compression/gzip.h"// FOR TESTING
#include <csignal>
#include <stdexcept>
#include <unistd.h>

void handle_alarm(int sig) {
    std::cerr << "Program timed out after 200 seconds. Exiting...\n";
    exit(0);// Exit cleanly, writing gmon.out
}
int main() {
    usub::server::component::Gzip gzip = usub::server::component::Gzip();
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, nullptr);
    signal(SIGALRM, handle_alarm);

    // Set alarm for 30 seconds
//    alarm(200);

    // std::signal(SIGINT, interruptSignalHandler);
    // std::signal(SIGTERM, interruptSignalHandler);
    usub::server::Server server_no_ssl("../../config/config.toml");
    server_no_ssl.addMiddleware(usub::server::protocols::http::MiddlewarePhase::HEADER, globalMiddle);

    // usub::server::Server server("/root/projects/serverMoving/confSSL.toml");
    server_no_ssl.handle("*", R"(/(.*))", handlerFunction).addMiddleware(usub::server::protocols::http::MiddlewarePhase::HEADER, headerMiddle).addMiddleware(usub::server::protocols::http::MiddlewarePhase::RESPONSE, responseMiddle);

    // Start a thread to stop the server after 110 seconds
   // std::thread stop_thread([&server_no_ssl]() {
   //     std::this_thread::sleep_for(std::chrono::seconds(110));
   //     std::cerr << "Stopping server after 110 seconds...\n";
   //     server_no_ssl.stop();
   // });
   // stop_thread.detach();

    server_no_ssl.run();

    return 0;
}

#if 0
#include "system/SystemContext.h"
#include "tasks/AwaitableFrame.h"
#include <iostream>

using namespace usub::uvent;

task::Awaitable<int> test() {
    co_return 42;
}

task::Awaitable<void> test_3() {
	std::cout<<333<<'\n';
	co_return;
}

task::Awaitable<void> test_2() {
    std::cout << 111 << '\n';
    co_await test_3();
    co_return;
}

int main() {
    system::co_spawn(test());
    auto coro = test();
    std::cout << "Result: " << coro.await_resume() << '\n';

    auto coro_2 = test_2();
    coro_2.await_resume();
}
#endif
