#pragma once

#include <cstddef>
#include <functional>
#include <unordered_map>
#include <utility>

template <typename... Args>
class Observable {
  using Id = std::size_t;
  using Function = std::function<void(Args...)>;

  std::unordered_map<Id, Function> subscriptions;
  Id nextId = 0;

public:
  Id subscribe(Function fn) {
    subscriptions[nextId] = std::move(fn);
    return nextId++;
  }

  void unsubscribe(Id id) {
    subscriptions.erase(id);
  }

  void publish(const Args &...args) {
    for (auto &&[id, fn] : subscriptions) {
      fn(args...);
    }
  }
};