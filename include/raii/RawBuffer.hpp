template <typename T>
class RawBuffer {
  T *data = nullptr;
  std::size_t size = 0;

public:
  explicit RawBuffer(std::size_t size) :
    data(new T[size]),
    size(size) {}

  virtual ~RawBuffer() {
     delete[] data;
  }

  RawBuffer(const RawBuffer &that) : 
    data(new T[that.size]),
    size(that.size) {
    try {
      std::copy_n(that.data, size, data);
    } catch (...) {
      delete[] data;
      throw;
    }
  }

  RawBuffer &operator=(const RawBuffer &that) {
    if (this != &that) {
      // 1. Allocate new buffer (may throw std::bad_alloc)
      T *newData = new T[that.size];
      // 2. Copy memory (may throw if T's constructor throws)
      std::copy_n(that.data, size, data);
      // 3. Deallocate old data (non-throw operation)
      delete[] data;
      // 3. Assign data and new size (non-throw operation)
      data = newData;
      size = that.size;
    }
    return *this;
  }

  RawBuffer(RawBuffer &&that) noexcept :
    data(std::exchange(that.data, nullptr)),
    size(std::exchange(that.size, 0)) {}

  RawBuffer &operator=(RawBuffer &&that) noexcept {
    if (this != &that) {
      delete[] data;
      size = std::exchange(that.size, 0);
      data = std::exchange(that.data, nullptr);
    }
    return *this;
  }
};