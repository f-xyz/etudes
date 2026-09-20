#pragma once

template <typename T>
class CopyAndSwapBuffer {
  std::unique_ptr<T[]> data;
  std::size_t size;

public:
  explicit CopyAndSwapBuffer(std::size_t size) :
    data(std::make_unique<T[]>(size)),
    size(size) {}

  CopyAndSwapBuffer(const CopyAndSwapBuffer<T> &that) :
    data(that.size > 0 ? std::make_unique<T[]>(that.size) : nullptr),
    size(that.size)  {
    std::copy(that.data.get(), that.data.get() + size, data.get());
  }

  CopyAndSwapBuffer(CopyAndSwapBuffer<T> &&that) noexcept :
    data(std::exchange(that.data, nullptr)),
    size(std::exchange(that.size, 0)) {}

  CopyAndSwapBuffer<T> &operator=(CopyAndSwapBuffer<T> that) noexcept {
    swap(*this, that);
    return *this;
  }

  friend void swap(CopyAndSwapBuffer<T> &a, CopyAndSwapBuffer<T> &b) noexcept {
    using std::swap;
    swap(a.size, b.size);
    swap(a.data, b.data);
  }
};