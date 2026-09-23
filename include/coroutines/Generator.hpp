#pragma once

#include <concepts>
#include <generator>
#include <print>

template <std::integral T> struct Generator {
  std::generator<T> generate(T max) {
    for (T i = 0; i < max; ++i) {
      co_yield i;
    }
  }

  static void print(T max) {
    for (auto x : Generator<int>().generate(max)) {
      std::println("{}", x);
    }
  }
};