#pragma once

template <typename T>
class StdBuffer {
public:
  std::unique_ptr<T[]> data;
  std::size_t size;

  explicit StdBuffer(std::size_t size) :
    data(std::make_unique<T[]>(size)),
    size(size) {}

  StdBuffer(const StdBuffer &that) :
    data(std::make_unique<T[]>(that.size)),
    size(that.size) {
      std::copy_n(that.data.get(), that.size, data.get());
    }

  StdBuffer(StdBuffer &&that) noexcept :
    data(std::move(that.data)),
    size(std::exchange(that.size, 0)) {}

  StdBuffer &operator=(const StdBuffer &that) {
    if (this != &that) {
      // 1. Allocate new buffer (may throw std::bad_alloc)
      const auto buffer = std::make_unique<T[]>(that.size);
      // 2. Copy memory (may throw if T's constructor throws)
      std::copy_n(that.data.get(), that.size, buffer.get());
      // 3. Assign data and new size (non-throw operation)
      data = std::move(buffer);
      size = that.size;
    }
    return *this;
  }

  StdBuffer &operator=(StdBuffer &&that) noexcept {
    if (this != &that) {
      data = std::move(that.data);
      size = std::exchange(that.size, 0);
    }
    return *this;
  }
};