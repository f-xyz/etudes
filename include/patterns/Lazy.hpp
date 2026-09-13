#pragma once

#include <optional>
#include <utility>

template <typename T, typename... Args>
class Lazy {
  std::optional<T> value;

public:
  T &get(Args &&...args) {
    if (!value.has_value()) {
      value.emplace(std::forward<Args>(args)...);
    }
    return *value;
  }

  const T &get() const { return const_cast<Lazy *>(this)->get(); }

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