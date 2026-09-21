#pragma once

#include <concepts>
#include <cstdint>
#include <format>
#include <string_view>

enum class Level : std::uint8_t { Debug, Info, Warning, Error };

template <typename T>
concept Filter = requires(const T &t, Level level) {
  { t.shouldLog(level) } -> std::same_as<bool>;
};

template <typename T>
concept Formatter = requires(const T &t, std::string_view line) {
  { t.format(line) } -> std::same_as<std::string>;
};

template <typename T>
concept Writer = requires(T &t, std::string_view line) {
  { t.write(line) } -> std::same_as<void>;
};

////////////////////////////////////////

class NoFilter {
public:
  bool shouldLog(Level) const { return true; }
};

class DefaultFormatter {
public:
  std::string format(std::string_view line) const {
    return std::format("Formatted: {}", line);
  }
};

class ConsoleWriter {
public:
  void write(std::string_view line) { std::println("ConsoleWriter: {}", line); }
};

////////////////////////////////////////

template <Filter Filter = NoFilter, Formatter Formatter = DefaultFormatter,
          typename Writer = ConsoleWriter>
class Logger : Filter, Formatter, Writer {
  using Filter::shouldLog;
  using Formatter::format;
  using Writer::write;

public:
  template <typename... Args>
  void log(Level level, std::string_view msg, Args &&...args) {
    if (shouldLog(level)) {
      const auto formatArgs =
          std::make_format_args(std::forward<Args...>(args)...);
      const auto line = std::vformat(msg, formatArgs);
      const auto formatted = format(line);
      write(formatted);
    }
  }
};
