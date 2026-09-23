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
#include <iterator>
#include <math.hpp>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>

using namespace std::chrono_literals;
using std::chrono::milliseconds;
using std::chrono::seconds;
using utils::benchmarking::Timer;
using utils::math::random;

class ThreadPoolWorkStealing {
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

private:
  void worker(std::size_t id) {
    auto &worker = workers[id];
    cl.log("Thread {} started", worker.id);

    while (true) {
      Task task;

      {
        cl.log("Thread {} sleeps", id);
        std::unique_lock lock(mutex);
        cv.wait(lock, [this, &worker] { return waiter(worker); });

        if (isShuttingDown(worker)) {
          break;
        }

        cl.log("Thread {} awakens with {} tasks", id, worker.queue.size());

        task = std::move(worker.queue.back());
        cl.log("Thread {} current task ID: {}", id, task.id);
        worker.queue.pop_back();
      }

      task();
    }
  }

  bool waiter(Worker &worker) {
    // If shutting down, unblock the thread
    if (!isRunning) {
      return true;
    }

    // If the thread has work to do
    if (worker.queue.size() > 0) {
      return true;
    }

    // Or other threads have some work
    auto randomWorkerIds = getRandomWorkerIds();
    for (const auto &randomWorkerId : randomWorkerIds) {
      if (randomWorkerId == worker.id) {
        continue;
      }

      auto &victim = workers[randomWorkerId];
      if (victim.nTasks() > 0) {
        cl.log("Thread {} steals a task from {}", worker.id, victim.id);
        auto task = std::move(victim.queue.front());
        victim.queue.pop_front();
        worker.queue.emplace_back(std::move(task));
        return true;
      }
    }

    return false;
  }

  std::vector<std::size_t> getRandomWorkerIds() {
    std::vector<std::size_t> randomWorkerIds;
    randomWorkerIds.reserve(workers.size());

    std::ranges::transform(workers, std::back_inserter(randomWorkerIds),
                           &Worker::id);

    thread_local std::random_device rd;
    thread_local std::mt19937 mt(rd());
    std::ranges::shuffle(randomWorkerIds, mt);

    return randomWorkerIds;
  }

  bool isShuttingDown(const Worker &worker) {
    return !isRunning && worker.queue.empty();
  }
};
