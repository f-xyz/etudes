#pragma once

#include <print>
#include <string_view>

template <typename Derived>
class BaseLogger {
  friend Derived;
  BaseLogger() = default;

public:
  void write(std::string_view message) {
    static_cast<Derived&>(*this).write(message);
  }
};

class ConsoleLogger : public BaseLogger<ConsoleLogger> {
public:
  void write(std::string_view message) {
    std::println("ConsoleLogger: {}", message);
  }
};

class FileLogger : public BaseLogger<FileLogger> {
public:
  void write(std::string_view message) {
    std::println("FileLogger: {}", message);
  }
};

////////////////////////////////////////

template <typename T>
void write(BaseLogger<T> &logger, std::string_view message) {
  logger.write(message);
}