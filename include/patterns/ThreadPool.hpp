#include "helpers/ColorLogger.hpp"
#include <TerminateHandler.hpp>
#include <algorithm>
#include <async/setTimeout.hpp>
#include <benchmarking/Timer.hpp>
#include <chrono>
#include <cli/colors.hpp>
#include <condition_variable>
#include <fitsio.h>
#include <format>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>

using namespace std::chrono_literals;
using std::chrono::seconds;
using utils::benchmarking::Timer;

class ThreadPool final {
  std::queue<std::packaged_task<void()>> queue;
  std::vector<std::jthread> threads;
  std::mutex mutex;
  std::condition_variable cv;
  std::atomic<bool> isRunning = true;
  ColorLogger cl {{0x008800, 0x888800, 0x880088, 0x008888}};

public:
  explicit ThreadPool(std::size_t n) {
    threads.reserve(n);

    for (std::size_t id = 0; id < n; ++id) {
      threads.emplace_back([this, id] { worker(id); });
    }
  }

  ThreadPool(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;

  ~ThreadPool() {
    std::println("----------------");
    std::println("Shutting down...");

    {
      std::scoped_lock lock(mutex);
      isRunning = false;
    }

    cv.notify_all();
  }

  void worker(std::size_t id) {
    cl.log("Thread {} started with {} tasks", id, queue.size());

    while (true) {
      cl.log("Thread {} sleeps with {} tasks", id, queue.size());

      std::packaged_task<void()> task;

      {
        std::unique_lock lock(mutex);
        cv.wait(lock, [this] { return queue.size() > 0 || !isRunning; });

        if (!isRunning && queue.empty()) {
          break;
        }

        cl.log("Thread {} awakens with {} tasks", id, queue.size());
        task = std::move(queue.front());
        queue.pop();
      }

      task();
    }
  }

  template <typename F, typename... Args> auto submit(F &&fn, Args &&...args) {
    using Result = std::invoke_result_t<F, Args...>;

    auto bound =
        std::bind_front(std::forward<F>(fn), std::forward<Args>(args)...);
    auto task = std::packaged_task<Result()>(std::move(bound));
    auto future = task.get_future();

    {
      std::scoped_lock lock(mutex);
      queue.emplace([task = std::move(task)] mutable { task(); });
    }

    cv.notify_one();

    return future;
  }
};
