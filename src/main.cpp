#include "raii/StdBuffer.hpp"
#include <cli/hexdump.hpp>
#include <numeric>

int main(const int, const char **) {
  std::println("Hello");

  StdBuffer<char> buffer(100);
  std::ranges::iota(buffer.data.get(), buffer.data.get() + 100, 0);
  auto dump = utils::cli::hexdump(buffer.data.get(), buffer.size);
  std::println("{}", dump);

  return 0;
}