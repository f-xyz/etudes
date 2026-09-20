#include "patterns/ThreadPool.hpp"
#include "patterns/ThreadPoolWorkStealing.hpp"
#include <TerminateHandler.hpp>
#include <benchmarking/Timer.hpp>
#include <cli/colors.hpp>
#include <thread>

using namespace std::chrono_literals;

int main(const int, const char **) {
  // runThreadPool();
  runThreadPoolWorkStealing();

  std::this_thread::sleep_for(500ms);

  return 0;
}