#pragma once

#include <functional>
#include <optional>

template <typename T>
class Lazy {
  std::optional<T> value;
  std::function<T()> factory;

public:
  explicit Lazy(std::function<T()> factory)
    : factory(std::move(factory)) {}

  T &get() {
    if (!value.has_value()) {
      value.emplace(factory());
    }
    return *value;
  }

  const T &get() const { return get(); }

  // Pointer-like syntax
  T &operator*() { return get(); }
  T *operator->() { return &get(); }
  const T &operator*() const { return get(); }
  const T *operator->() const { return &get(); }

  // Conversion
  operator T &() & { return get(); }
  operator T &&() && { return std::move(get()); }
  explicit operator bool() const { return value.has_value(); }
};