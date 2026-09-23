#include "coroutines/TaskAndAwaiter.hpp"

int main(const int, const char **) {
  auto gen = coroutine();

  std::println("{}", gen.get());
  gen.next();

  std::println("{}", gen.get());
  gen.next();

  std::println("{}", gen.get());
  gen.next();

  return 0;
}