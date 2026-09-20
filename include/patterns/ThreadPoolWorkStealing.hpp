#pragma once

#include "helpers/ColorLogger.hpp"
#include <TerminateHandler.hpp>
#include <algorithm>
#include <async/setTimeout.hpp>
#include <benchmarking/Timer.hpp>
#include <chrono>
#include <cli/colors.hpp>
#include <condition_variable>
#include <deque>
#include <format>
#include <functional>
#include <future>
#include <math.hpp>
#include <mutex>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>

using namespace std::chrono_literals;
using std::chrono::milliseconds;
using std::chrono::seconds;
using utils::benchmarking::Timer;
using utils::math::random;

class ThreadPoolWorkStealing final {
  struct Task {
    std::size_t id;
    std::packaged_task<void()> task;
    void operator()() { task(); }
  };

  struct Worker {
    std::size_t id;
    std::jthread thread;
    std::deque<Task> queue;
    std::size_t nTasks() { return queue.size(); }
  };

  std::size_t lastTaskId = 0;
  std::vector<Worker> workers;
  std::mutex mutex;
  std::condition_variable cv;
  std::atomic<bool> isRunning = true;
  ColorLogger cl {{0x008800, 0x888800, 0x880088, 0x008888}};

public:
  explicit ThreadPoolWorkStealing(std::size_t n) {
    workers.reserve(n);

    for (std::size_t id = 0; id < n; ++id) {
      workers.emplace_back(
          Worker {.id = id,
                  .thread = std::jthread([this, id] { worker(id); }),
                  .queue = std::deque<Task>()});
    }
  }

  ThreadPoolWorkStealing(const ThreadPoolWorkStealing &) = delete;
  ThreadPoolWorkStealing(ThreadPoolWorkStealing &&) = delete;
  ThreadPoolWorkStealing &operator=(const ThreadPoolWorkStealing &) = delete;
  ThreadPoolWorkStealing &operator=(ThreadPoolWorkStealing &&) = delete;

  ~ThreadPoolWorkStealing() {
    std::println("----------------");
    std::println("Shutting down...");

    {
      std::scoped_lock lock(mutex);
      isRunning = false;
    }

    cv.notify_all();
  }

  void worker(std::size_t id) {
    auto &worker = workers[id];
    cl.log("Thread {} started", worker.id);

    while (true) {
      Task task;

      {
        cl.log("Thread {} sleeps", id);
        std::unique_lock lock(mutex);
        cv.wait(lock, [this, &worker] { return waiter(worker); });

        if (!isRunning && worker.queue.empty()) {
          break;
        }

        cl.log("Thread {} awakens with {} tasks", id, worker.queue.size());

        task = std::move(worker.queue.back());
        cl.log("Thread {} current task ID: {}", id, task.id);
        worker.queue.pop_back();
      }

      task();
    }

    cl.log("Thread {} ended", id);
  }

  bool waiter(Worker &worker) {
    if (!isRunning) {
      return true;
    }

    if (worker.queue.size() > 0) {
      return true;
    }

    auto mostLoadedWorker =
        std::ranges::max_element(workers, std::less(), &Worker::nTasks);

    if (mostLoadedWorker->id != worker.id && mostLoadedWorker->nTasks() > 0) {
      cl.log("Thread {} steals a task from {}", worker.id,
             mostLoadedWorker->id);
      auto task = std::move(mostLoadedWorker->queue.front());
      mostLoadedWorker->queue.pop_front();
      worker.queue.emplace_back(std::move(task));
      return true;
    }

    return false;
  }

  template <typename F, typename... Args> auto submit(F &&fn, Args &&...args) {
    using Result = std::invoke_result_t<F, Args...>;

    auto func =
        std::bind_front(std::forward<F>(fn), std::forward<Args>(args)...);
    auto task = std::packaged_task<Result()>(std::move(func));
    auto future = task.get_future();

    {
      std::scoped_lock lock(mutex);

      const auto randomWorkerId = random<std::size_t>(0, workers.size() - 1);
      auto &worker = workers[randomWorkerId];
      // auto &worker = workers[0];

      cl.log("Thread {} takes the task", worker.id);
      auto wrapper = std::packaged_task<void()>(
          [task = std::move(task)] mutable { task(); });
      worker.queue.push_back(
          Task {.id = lastTaskId++, .task = std::move(wrapper)});
    }

    cv.notify_one();

    return future;
  }
};

void runThreadPoolWorkStealing() {
  TerminateHandler::install();
  ThreadPoolWorkStealing pool(2);
  Timer<milliseconds> timer;

  const std::size_t nTasks = 4;
  std::vector<std::future<std::string>> futures;

  for (std::size_t i = 0; i < nTasks; ++i) {
    std::this_thread::sleep_for(100ms);
    std::println("Task {}: submitting...", i);

    auto future = pool.submit([i] {
      const int randomTime = static_cast<int>(random(500, 1500));
      std::chrono::milliseconds ms(randomTime);
      std::this_thread::sleep_for(ms);

      std::println("Task {}: finished by thread {}", i,
                   std::this_thread::get_id());

      const auto ch = static_cast<char>('0' + i);
      return std::string(3, ch);
    });

    futures.push_back(std::move(future));
  }

  for (std::size_t i = 0; i < nTasks; ++i) {
    std::println("Task {}: result: {}", i, futures[i].get());
  }

  std::println("Finished in {}", timer.measure());
}