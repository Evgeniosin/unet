#include "globals.h"
#include "logging/logger.h"

std::vector<event_base *> eventBases;
std::vector<std::thread> server_threads;
std::mutex eventBasesMutex;
std::atomic<bool> signalReceived(false);

void stopAllEventBases() {
    std::lock_guard<std::mutex> lock(eventBasesMutex);
    signalReceived.store(true);
    info_log("Stopping event bases.");
    for (auto base : eventBases) {
         event_base_loopbreak(base);
    }
    info_log("Joining threads.");
    for (auto& thread : server_threads) {
        thread.join();
    } 
}

void interruptSignalHandler(int signum) {
    stopAllEventBases();
}

