#pragma once

#include <astroutils/math.hpp>
#include <thread>

using namespace std::chrono_literals;
using astroutils::math::random;

template <typename T>
concept HasSubmit = requires(T &pool) { pool.submit([] {}); };

template <HasSubmit Pool> class ThreadPoolLoader {
public:
  void generateLoad(Pool &pool, const std::size_t nTasks) {
    std::vector<std::future<std::string>> futures;

    for (std::size_t i = 0; i < nTasks; ++i) {
      std::println("Task {}: submitting...", i);
      auto future = pool.submit([this, i] { return task(i); });
      futures.push_back(std::move(future));
    }

    for (std::size_t i = 0; i < nTasks; ++i) {
      std::println("Task {}: result: {}", i, futures[i].get());
    }
  }

private:
  auto task(std::size_t i) {
    const int randomTime = static_cast<int>(random(500, 1500));
    std::chrono::milliseconds ms(randomTime);
    std::this_thread::sleep_for(ms);

    std::println("Task {}: finished by thread {}", i,
                 std::this_thread::get_id());

    const auto ch = static_cast<char>('0' + i);
    return std::string(3, ch);
  }
};