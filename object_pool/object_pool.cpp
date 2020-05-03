#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <new>
#include <string>
#include <queue>
#include <stdexcept>
#include <set>
#include <unordered_map>
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
    auto it = active.find(object);
    if (it == end(active)) {
      throw invalid_argument("Pool has no free objects");
    }
    free.push(move(it->second));
    active.erase(it);
  }

private:
  T* GetFromFreeQueue() {
    auto ptr = move(free.front());
    free.pop();
    auto ret = ptr.get();
    active[ret] = move(ptr);
    return ret;
  }

  queue<unique_ptr<T>> free;
  unordered_map<T*, unique_ptr<T>> active;
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

void run() {
  static int counter{0};
  struct Counted {
    Counted() { counter++; }
    ~Counted() { counter--; }
  };

  {
    ObjectPool<Counted> pool;

    cout << "Counter before loop = " << counter << endl;

    try {
      for (int i = 0; i < 1'000; ++i) {
        cout << "Allocating object #" << i << endl;
        pool.Allocate();
      }
    } catch (const std::bad_alloc& e) {
      cout << e.what() << endl;
    }

    cout << "Counter after loop = " << counter << endl;
  }
  cout << "Counter before exit = " << counter << endl;
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestObjectPool);

  run();

  return 0;
}
