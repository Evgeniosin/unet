#ifndef GLOBALS_H
#define GLOBALS_H
#include <vector>
#include <mutex>
#include <thread>
#include <event.h>
#include <atomic>

extern std::vector<event_base *> eventBases;
extern std::vector<std::thread> server_threads;
extern std::mutex eventBasesMutex;
extern std::atomic<bool> signalReceived;

void stopAllEventBases();

void interruptSignalHandler(int signum);

#endif // GLOBALS_H
