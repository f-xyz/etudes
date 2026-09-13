#include <concepts>
#include <memory>
#include <utility>

template <typename T>
concept DoesStuff = requires(T x) {
  { x.doStuff() } -> std::same_as<void>;
};

class Dynamic {
  struct Concept {
    virtual ~Concept() = default;
    virtual void doStuff() = 0;
    virtual std::unique_ptr<Concept> clone() = 0;
  };

  template <DoesStuff T>
  struct Model : Concept {
    T t;
    Model(T t) : t(std::move(t)) {}
    virtual void doStuff() override { t.doStuff(); }
    virtual std::unique_ptr<Concept> clone() override {
      return std::make_unique<Model<T>>(t);
    }
  };

  std::unique_ptr<Concept> pimpl;

public:
  template <DoesStuff T>
  Dynamic(T x) 
    : pimpl(std::make_unique<Model<T>>(x)) {}

  Dynamic(const Dynamic &that)
    : pimpl(that.pimpl ? that.pimpl->clone() : nullptr) {}
  
  Dynamic &operator=(const Dynamic &that) {
    if (this != &that) {
      pimpl = that.pimpl ? that.pimpl->clone() : nullptr;
    }
    return *this;
  }

  Dynamic(Dynamic &&) noexcept = default;
  Dynamic &operator=(Dynamic &&) noexcept = default;

  void doStuff() const { pimpl->doStuff(); }
};