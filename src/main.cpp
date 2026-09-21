#include "helpers/ThreadPoolLoader.hpp"
#include "patterns/ThreadPool.hpp"
#include "patterns/ThreadPoolWorkStealing.hpp"
#include <TerminateHandler.hpp>
#include <benchmarking/Timer.hpp>
#include <cli/colors.hpp>

using namespace std::chrono_literals;

int main(const int, const char **) {
  TerminateHandler::install();
  ThreadPool pool(4);
  // ThreadPoolWorkStealing pool(4);

  Timer<seconds> timer;
  ThreadPoolLoader<decltype(pool)> loader;
  loader.generateLoad(pool, 4);
  std::println("Finished in {}", timer.measure());

  return 0;
}