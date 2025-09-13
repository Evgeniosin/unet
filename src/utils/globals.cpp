#include "utils/globals.h"

std::atomic<bool> do_kill(false);
std::atomic<uint64_t> counter = 0;
// void stopAllEventBases() {
//     std::lock_guard<std::mutex> lock(eventBasesMutex);
//     signalReceived.store(true);
//     info_log("Stopping event bases.");
//     for (auto base : eventBases) {
//          event_base_loopbreak(base);
//     }
//     info_log("Joining threads.");
//     for (auto& thread : server_threads) {
//         thread.join();
//     }
// }

// void stopAllEventBases() {
//     info_log("Signal caught");
//     signalReceived.store(true);
// }

// void interruptSignalHandler(int signum) {
//     stopAllEventBases();
// }
