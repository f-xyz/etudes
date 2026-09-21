#pragma once

#include <cli/colors.hpp>

class ColorLogger {
  std::vector<unsigned int> colors;

public:
  ColorLogger(const std::vector<unsigned int> &colors) : colors(colors) {};

  template <typename... Args>
  void log(const std::string &msg, std::size_t id, Args &&...args) {
    const auto color = colors[id % colors.size()];
    const auto formatArgs = std::make_format_args(id, args...);
    const auto line = std::vformat(msg, formatArgs);

    std::println("{}", utils::cli::rgb(line, color));
  }
};