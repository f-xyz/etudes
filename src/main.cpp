#include "patterns/CRTP.hpp"
#include <variant>

int main(const int, const char **) {
  using Logger = std::variant<ConsoleLogger, FileLogger>;

  std::vector<Logger> v;
  v.push_back(ConsoleLogger());
  v.push_back(FileLogger());

  for (auto &&logger : v) {
    std::visit([](auto &&logger) {
      write(logger, "Hello");
    }, logger);
  }

  return 0;
}