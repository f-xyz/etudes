#include <coroutine>
#include <utility>

// The external interface for the caller
template <typename T> struct Task {

  // The internal controller owned by the coroutine state machine
  struct promise_type {
    T value {};

    Task get_return_object() noexcept {
      return Task {std::coroutine_handle<promise_type>::from_promise(*this)};
    };

    // Coroutine starts
    std::suspend_never initial_suspend() noexcept { return {}; }

    // co_yield is called with a raw value
    std::suspend_always yield_value(T v) noexcept {
      value = std::move(v);
      return {};
    }

    // co_yield is called with an Awaiter<T>
    template <typename Awaiter>
    Awaiter yield_value(Awaiter &&awaiter) noexcept {
      value = awaiter.result;
      return std::forward<Awaiter>(awaiter);
    }

    // Coroutine finishes
    std::suspend_always final_suspend() noexcept { return {}; }

    // co_return is called
    void return_value(T v) noexcept { value = std::move(v); }

    // An exception is thrown
    void unhandled_exception() { std::terminate(); }
  };

  // A wrapped pointer to the coroutine's state on the heap
  std::coroutine_handle<promise_type> handle;

  // Lifetime / RAII
  explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}
  Task(const Task &) = delete;
  Task &operator=(const Task &) = delete;
  Task(Task &&that) noexcept : handle(std::exchange(that.handle, nullptr)) {}
  Task &operator=(Task &&that) noexcept {
    if (this != &that) {
      if (handle) {
        handle.destroy();
      }
      handle = std::exchange(that.handle, nullptr);
    }
    return *this;
  }

  ~Task() {
    if (handle) {
      handle.destroy();
    }
  }

  // The return value
  T get() const { return handle.promise().value; }

  void next() {
    if (handle && !handle.done()) {
      handle.resume();
    }
  }
};

// The bridge for pausing and resuming
template <typename T> struct Awaiter {
  T result;

  Awaiter(T result) : result(result) {}

  // Is the result already awailable?
  bool await_ready() noexcept { return false; }

  // What shoudld happend right after pausing?
  void await_suspend(std::coroutine_handle<>) noexcept {}

  // What does the co_await evaluate to?
  T await_resume() noexcept { return result; }
};

Task<int> coroutine() {
  co_yield 123;
  co_yield Awaiter<int>(456);
  co_return 789;
}