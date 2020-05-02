#include "test_runner.h"

#include <functional>
#include <string>
#include <optional>

using namespace std;

template <typename T>
class LazyValue {
public:
  explicit LazyValue(std::function<T()> init) : init_func(init) {}

  bool HasValue() const { return stored_value.has_value(); }

  const T& Get() const {
    if (!HasValue()) {
      stored_value = init_func();
    }

    return stored_value.value();
  }

private:
  mutable optional<T> stored_value{nullopt};
  function<T()> init_func;
};

void UseExample() {
  const string big_string = "Giant amounts of memory";

  LazyValue<string> lazy_string([&big_string] { return big_string; });

  ASSERT(!lazy_string.HasValue());
  ASSERT_EQUAL(lazy_string.Get(), big_string);
  ASSERT_EQUAL(lazy_string.Get(), big_string);
}

void TestInitializerIsntCalled() {
  bool called = false;

  {
    LazyValue<int> lazy_int([&called] {
      called = true;
      return 0;
    });
  }
  ASSERT(!called);
}

void TestInitedAtGetCalling() {
  bool called = false;

  LazyValue<int> lazy_int([&called] {
    called = true;
    return 42;
  });
  ASSERT(!called);

  int value = lazy_int.Get();
  ASSERT(called);
  ASSERT_EQUAL(value, 42);
}

void TestInitCalledOnce() {
  int count = 0;

  LazyValue<int> lazy_int([&count] {
    count += 1;
    return 0;
  });
  ASSERT_EQUAL(count, 0);

  lazy_int.Get();
  ASSERT_EQUAL(count, 1);

  lazy_int.Get();
  ASSERT_EQUAL(count, 1);
}

void TestNoDefaultConstructor() {
  class Widget {
  public:
    Widget() = delete;
    Widget(int x) : value(x) {}
  private:
    int value;
  };

  LazyValue<Widget> lasy_widget([] { return Widget(42); });
  ASSERT(true); // check only that the code above is compiled
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, UseExample);
  RUN_TEST(tr, TestInitializerIsntCalled);
  RUN_TEST(tr, TestInitedAtGetCalling);
  RUN_TEST(tr, TestInitCalledOnce);
  RUN_TEST(tr, TestNoDefaultConstructor);
  return 0;
}
