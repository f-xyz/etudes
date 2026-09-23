#include "patterns/Lazy.hpp"

struct Q {
  int value;
  explicit Q(int v) : value(v) {}
  void f() const {
    std::println("Q.f(): {}", 123);
  }
};

void lvalue(const Q &q) { q.f(); }
void rvalue(Q&& q) { q.f(); }

int main(const int, const char **) {
  Lazy<Q> lazy([] { return Q(123); });
  const Lazy<Q> clazy([] { return Q(465); });

  lvalue(lazy);
  rvalue(std::move(lazy));

  return 0;
}