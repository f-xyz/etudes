#pragma once

// 1. Plain data structures (no common base class, no virtual functions)
struct Circle {};
struct Square {};

// 2. Structural collection container
using Shape = std::variant<Circle, Square>;

// 3. Helper utility for inline visitor lambdas
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void runVisitorVariant() {
  Shape shape = Square {};

  // 4. Visit a shape
  std::visit(overloaded {
    [](const Circle& c) { std::println("Circle"); },
    [](const Square& s) { std::println("Square"); }
  }, shape);
}