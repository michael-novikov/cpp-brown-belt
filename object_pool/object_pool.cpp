#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <new>
#include <string>
#include <queue>
#include <stdexcept>
#include <unordered_set>
using namespace std;

template <class T>
class ObjectPool {
public:
  T* Allocate() {
    if (free.empty()) {
      free.push(make_unique<T>());
    }
    return GetFromFreeQueue();
  }

  T* TryAllocate() {
    if (free.empty()) {
      return nullptr;
    }
    return GetFromFreeQueue();
  }

  void Deallocate(T* object) {
    unique_ptr<T> ptr{object};
    auto it = allocated.find(ptr);
    ptr.release();
    if (it == end(allocated)) {
      throw invalid_argument("Pool has no free objects");
    }
    free.push(move(allocated.extract(it).value()));
  }

private:
  T* GetFromFreeQueue() {
    auto ptr = move(free.front());
    free.pop();
    auto ret = ptr.get();
    allocated.insert(move(ptr));
    return ret;
  }

  struct Compare {
    using is_transparent = void;

    bool operator()(const unique_ptr<T>& lhs, const unique_ptr<T>& rhs) const {
      return lhs < rhs;
    }

    bool operator()(const unique_ptr<T>& rhs, const T* lhs) const {
      return less<const T*>()(rhs.get(), lhs);
    }

    bool operator()(const T* lhs, const unique_ptr<T>& rhs) const {
      return less<const T*>()(lhs, rhs.get());
    }
  };

  queue<unique_ptr<T>> free;
  unordered_set<unique_ptr<T>> allocated;
};

void TestObjectPool() {
  ObjectPool<string> pool;

  auto p1 = pool.Allocate();
  auto p2 = pool.Allocate();
  auto p3 = pool.Allocate();

  *p1 = "first";
  *p2 = "second";
  *p3 = "third";

  pool.Deallocate(p2);
  ASSERT_EQUAL(*pool.Allocate(), "second");

  pool.Deallocate(p3);
  pool.Deallocate(p1);
  ASSERT_EQUAL(*pool.Allocate(), "third");
  ASSERT_EQUAL(*pool.Allocate(), "first");

  pool.Deallocate(p1);
}

void TestLeakage() {
  static const int max_objects{1'000};
  static int counter{0};
  struct Counted {
    Counted() { counter++; }
    ~Counted() { counter--; }
  };

  {
    ObjectPool<Counted> pool;

    // cout << "Counter before loop = " << counter << endl;
    ASSERT_EQUAL(counter, 0);

    try {
      for (int i = 0; i < max_objects; ++i) {
        // cout << "Allocating object #" << i << endl;
        pool.Allocate();
      }
    } catch (const std::bad_alloc& e) {
      cout << e.what() << endl;
    }

    // cout << "Counter after loop = " << counter << endl;
  }
  // cout << "Counter before exit = " << counter << endl;
  ASSERT_EQUAL(counter, 0);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestObjectPool);
  RUN_TEST(tr, TestLeakage);
  cout << "Please, run TestLeakage multiple times with memory limitation" << endl;

  return 0;
}
