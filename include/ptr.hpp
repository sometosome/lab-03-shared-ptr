// Copyright by NIK
#ifndef INCLUDE_PTR_HPP_
#define INCLUDE_PTR_HPP_

#include <string>
#include <memory>
#include <utility>
#include <atomic>
#include <fstream>
#include "sp_counter.hpp"
#include <iostream>

template <typename T>
class SharedPtr {
 public:
  SharedPtr() noexcept {
    ptr = nullptr;
    counter = nullptr;
  }
  explicit SharedPtr(T* r) {
    std::unique_ptr<T> p(r);
    counter = new SPCounter<T>(p.get());
    ptr = p.release();
  }
  SharedPtr(const SharedPtr& r) {
    if (std::is_move_constructible<T>::value){
      ptr = r.ptr;
      counter = r.counter;
      if (counter) {
        counter->add();
      }
    } else {
      throw std::runtime_error("Not constructible type!");
    }
  }
  SharedPtr(SharedPtr&& r) {
    if (std::is_move_assignable<T>::value) {
      std::swap(ptr, r.ptr);
      std::swap(counter, r.counter);
    } else {
      throw std::runtime_error("Not assignable type!");
    }
  }
  ~SharedPtr() noexcept {
    if (counter) {
      counter->release();
    }
  }
  auto operator=(const SharedPtr& r) -> SharedPtr& {
    if (std::is_move_constructible<T>::value && &r !=this) {
      if (counter) {
        counter->realease();
      }
      ptr = r.ptr;
      counter = r.counter;
      if (counter) {
        counter->add();
      }
    } else if (&r == this) {
      std::cout << "Object equal to this\n";
    } else {
      throw std::runtime_error("Not constructible type!");
    }
    return *this;
  }
  auto operator=(SharedPtr&& r) -> SharedPtr& {
    if (std::is_move_assignable<T>::value && &r !=this) {
      if (counter) {
        counter->realease();
      }
      ptr = r.ptr;
      counter = r.counter;
      if (counter) {
        counter->add();
      }
    } else if (&r == this) {
      std::cout << "Object equal to this\n";
    } else {
      throw std::runtime_error("Not assignable type!");
    }
    return *this;
  }

  operator bool() const { return (ptr != nullptr); }
  auto operator*() const -> T& { return *ptr; }
  auto operator->() const -> T* { return ptr; }

  auto get() -> T* { return ptr; }
  void reset() {
    if (counter) {
      if (counter->use_count() == 1) {
        counter->release();
      } else {
        counter->unadd();
      }
    }
    ptr = nullptr;
    counter = nullptr;
  }
  void reset(T* r) {
    if (counter) {
      if (counter->use_count() == 1) {
        counter->release();
      } else {
        counter->unadd();
      }
    }
    ptr = r;
    if (ptr == nullptr) {
      counter = nullptr;
    } else {
      if (counter) {
        counter->add();
      } else {
        std::unique_ptr<T> p(r);
        counter = new SPCounter<T>(p.get());
      }
    }
  }
  void swap(SharedPtr& r) {
    std::swap(r.ptr, ptr);
    std::swap(r.counter, counter);
  }

  size_t use_count() {
    if (counter){
      return counter->use_count();
    }else {
      return 0;
    }
  }

 private:
  T *ptr;
  SPCounter<T>* counter;
};

#endif // INCLUDE_PTR_HPP_

