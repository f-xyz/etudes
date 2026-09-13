template <typename T>
class RuleOfZeroBuffer {
  std::unique_ptr<T[]> data = nullptr;
  std::size_t size = 0;

public:
  explicit RuleOfZeroBuffer(std::size_t size) :
    data(std::make_unique<T[]>(size)),
    size(size) {}
};