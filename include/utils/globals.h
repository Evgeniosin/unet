#ifndef GLOBALS_H
#define GLOBALS_H
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>


extern std::mutex eventBasesMutex;
extern std::atomic<bool> do_kill;
extern std::atomic<uint64_t> counter;

void stopAllEventBases();

void interruptSignalHandler(int signum);

#endif// GLOBALS_H
