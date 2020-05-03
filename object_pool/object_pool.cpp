#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <set>
using namespace std;

template <class T>
class ObjectPool {
public:
  T* Allocate() {
    if (free.empty()) {
      free.push(new T);
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
    if (!active.count(object)) {
      throw invalid_argument("Pool has no free objects");
    }
    active.erase(object);
    free.push(object);
  }

  ~ObjectPool() {
    for (auto object : active) {
      delete object;
    }
    active.clear();

    while (!free.empty()) {
      auto object = free.front();
      free.pop();
      delete object;
    }
  }

private:
  T* GetFromFreeQueue() {
    auto object = free.front();
    free.pop();
    active.insert(object);
    return object;
  }

  queue<T*> free;
  set<T*> active;
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

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestObjectPool);
  return 0;
}
