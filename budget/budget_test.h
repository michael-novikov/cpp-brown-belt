#ifndef BUDGET_TEST_H
#define BUDGET_TEST_H

class TestDate {
public:
  static void TestAll();

  static void TestConstructor();
  static void TestValidate();
  static void TestAsTimestamp();
  static void TestFromString();
  static void TestComputeDaysDiff();
};

#endif // BUDGET_TEST_H
