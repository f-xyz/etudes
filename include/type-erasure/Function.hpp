#include <memory>
#include <utility>

// 1. Primary template declaration
template <typename T> class Function;

// 2. Specialized for function signatures: R(Args...)
template <typename R, typename... Args> class Function<R(Args...)> {

  // Abstract base class: Erases 'T', defines the polymorphic interface
  struct Concept {
    virtual ~Concept() = default;
    virtual R invoke(Args... args) = 0;
  };

  // Derived template: Remembers 'T' and overrides the interface
  template <typename F> struct Model final : Concept {
    F fn;
    explicit Model(F fn) : fn(std::move(fn)) {}
    R invoke(Args... args) override { return fn(std::forward<Args>(args)...); }
  };

  // Polymorphic pointer to erased state
  std::unique_ptr<Concept> pimpl;

public:
  // Templated constructor accepts ANY callable matching the signature
  template <typename F>
  Function(F fn)
    : pimpl(std::make_unique<Model<F>>(std::forward<F>(fn))) {}

  // Non-virtual public call operator
  R operator()(Args... args) const {
    return pimpl->invoke(std::forward<Args>(args)...);
  }
};