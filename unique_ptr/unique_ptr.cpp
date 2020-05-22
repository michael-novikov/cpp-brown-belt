#include "test_runner.h"

#include <cstddef>
#include <utility>

using namespace std;

template <typename T>
class UniquePtr {
private:
  T* data;

  UniquePtr(const UniquePtr&);
  UniquePtr& operator = (const UniquePtr&);
public:
  UniquePtr(): data(nullptr) {}
  UniquePtr(T * ptr): data(ptr) {}
  UniquePtr(UniquePtr&& other): data(move(other.data)) {}
  UniquePtr& operator = (nullptr_t);
  UniquePtr& operator = (UniquePtr&& other);
  ~UniquePtr() { delete data; }

  T& operator * () const { return *data; }

  T * operator -> () const { return data; }

  T * Release();

  void Reset(T * ptr);

  void Swap(UniquePtr& other);

  T * Get() const { return data; }
};

template <typename T>
UniquePtr<T>& UniquePtr<T>::operator = (nullptr_t) {
  T * old_data = data;
  data = nullptr;
  delete old_data;
}

template <typename T>
UniquePtr<T>& UniquePtr<T>::operator = (UniquePtr&& other) {
  T * old_data = data;
  data = move(other.data);
  delete old_data;
}

template <typename T>
T * UniquePtr<T>::Release() {
  T * tmp = data;
  data = nullptr;
  return tmp;
}

template <typename T>
void UniquePtr<T>::Reset(T * ptr) {
  T * old_data = data;
  data = ptr;
  delete old_data;
}

template <typename T>
void UniquePtr<T>::Swap(UniquePtr& other) {
  swap(data, other.data);
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

    auto rawPtr = ptr.Release();
    ASSERT_EQUAL(Item::counter, 1);

    delete rawPtr;
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

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestLifetime);
  RUN_TEST(tr, TestGetters);
}
