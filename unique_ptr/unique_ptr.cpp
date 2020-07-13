#include "test_runner.h"

#include <cstddef>
#include <utility>

using namespace std;

template <typename T>
class UniquePtr {
private:
  T* data;

public:
  UniquePtr(const UniquePtr&) = delete;
  UniquePtr& operator = (const UniquePtr&) = delete;

  UniquePtr(): data(nullptr) {}
  UniquePtr(nullptr_t): data(nullptr) {}
  UniquePtr(T * ptr): data(ptr) {}
  UniquePtr(UniquePtr&& other): data(other.Release()) {}
  UniquePtr& operator = (nullptr_t);
  UniquePtr& operator = (UniquePtr&& other);
  ~UniquePtr() { Reset(); }

  T& operator * () const { return *data; }

  T * operator -> () const { return data; }

  T * Release();

  void Reset(nullptr_t = nullptr);
  void Reset(T * ptr);

  void Swap(UniquePtr& other);

  T * Get() const;
};

template <typename T>
UniquePtr<T>& UniquePtr<T>::operator = (nullptr_t) {
  Reset();
  return *this;
}

template <typename T>
UniquePtr<T>& UniquePtr<T>::operator = (UniquePtr&& other) {
  UniquePtr<T>(other.Release()).Swap(*this);
  return *this;
}

template <typename T>
T * UniquePtr<T>::Release() {
  T * tmp = data;
  data = nullptr;
  return tmp;
}

template <typename T>
void UniquePtr<T>::Reset(nullptr_t) {
  T * tmp = data;
  data = nullptr;
  if (tmp) {
    delete tmp;
  }
}

template <typename T>
void UniquePtr<T>::Reset(T * ptr) {
  T * tmp = data;
  data = ptr;
  if (tmp) {
    delete tmp;
  }
}

template <typename T>
void UniquePtr<T>::Swap(UniquePtr& other) {
  swap(data, other.data);
}

template <typename T>
T * UniquePtr<T>::Get() const {
  return data;
}

struct Item {
  static int counter;
  int value;
  Item(int v = 0): value(v) {
    ++counter;
  }
  Item(const Item& other): value(other.value) {
    ++counter;
  }
  ~Item() {
    --counter;
  }
};

int Item::counter = 0;


void TestLifetime() {
  Item::counter = 0;
  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    ptr.Reset(new Item);
    ASSERT_EQUAL(Item::counter, 1);
  }
  ASSERT_EQUAL(Item::counter, 0);

  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    ASSERT(ptr.Get() != nullptr);
    auto releasedRawPtr = ptr.Release();
    ASSERT(ptr.Get() == nullptr);
    ASSERT_EQUAL(Item::counter, 1);

    delete releasedRawPtr;
    ASSERT_EQUAL(Item::counter, 0);
  }
  ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters() {
  UniquePtr<Item> ptr(new Item(42));
  ASSERT_EQUAL(ptr.Get()->value, 42);
  ASSERT_EQUAL((*ptr).value, 42);
  ASSERT_EQUAL(ptr->value, 42);
}

void TestGettersNullptr() {
  UniquePtr<Item> ptr;
  ASSERT(ptr.Get() == nullptr);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestLifetime);
  RUN_TEST(tr, TestGetters);
  RUN_TEST(tr, TestGettersNullptr);
}
