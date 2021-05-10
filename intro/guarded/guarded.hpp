#pragma once

#include <twist/stdlike/mutex.hpp>
#include <mutex>

namespace solutions {

// Automagically wraps all accesses to guarded object to critical sections
// Look at unit tests for API and usage examples

namespace {
template <typename GuardedT>
class MutexUnlocker : private std::lock_guard<twist::stdlike::mutex> {
 public:
  MutexUnlocker(twist::stdlike::mutex& mutex, GuardedT* object)
      : std::lock_guard<twist::stdlike::mutex>(mutex), object_(object) {
  }

  GuardedT* operator->() {
    return object_;
  }

 private:
  GuardedT* object_;
};
}  // namespace

template <typename T>
class Guarded {
 public:
  // https://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/
  template <typename... Args>
  Guarded(Args&&... args) : object_(std::forward<Args>(args)...) {
  }

  // https://en.cppreference.com/w/cpp/language/operators

  MutexUnlocker<T> operator->() {
    return MutexUnlocker<T>(mutex_, &object_);
  }

 private:
  T object_;
  twist::stdlike::mutex mutex_;  // Guards access to object_
};

}  // namespace solutions