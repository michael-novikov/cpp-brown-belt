#include "budget_test.h"

#include <iterator>
#include <vector>
#include <algorithm>
#include <numeric>

#include "budget_system.h"
#include "date.h"
#include "test_runner.h"

using namespace std;

void TestDate::TestConstructor() {
  {
    // First supported date
    Date date{2000, 1, 1};
    ASSERT_EQUAL(date.year_, 2000);
    ASSERT_EQUAL(date.month_, 1);
    ASSERT_EQUAL(date.day_, 1);
  }

  {
    // Last supported date
    Date date{2099, 12, 31};
    ASSERT_EQUAL(date.year_, 2099);
    ASSERT_EQUAL(date.month_, 12);
    ASSERT_EQUAL(date.day_, 31);
  }

  {
    // Invalid date: negative numbers
    ASSERT_THROWS(([] { return Date{-1, 1, 1}; }));
    ASSERT_THROWS(([] { return Date{2000, -1, 1}; }));
    ASSERT_THROWS(([] { return Date{2000, 1, -1}; }));
  }
}

void TestDate::TestValidate() {
  {
    // Supported range
    ASSERT(Date::Validate(2000, 1, 1));
    ASSERT(Date::Validate(2099, 12, 31));
  }

  {
    // Invalid date: negative numbers
    ASSERT(!Date::Validate(-1, 1, 1));
    ASSERT(!Date::Validate(2000, -1, 1));
    ASSERT(!Date::Validate(2000, 1, -1));
  }

  {
    // Test leap years from 2000 to 2099

    vector<int> supported_years(100);
    iota(begin(supported_years), end(supported_years), 2000);

    auto it = partition(
      begin(supported_years), end(supported_years),
      [] (int year) { return year % 4 == 0; }
    );

    vector<int> leap_years(25);
    vector<int> non_leap_years(75);

    copy(begin(supported_years), it, begin(leap_years));
    copy(it, end(supported_years), begin(non_leap_years));

    for (int year : leap_years) {
      ASSERT(Date::Validate(year, 2, 29));
    }

    for (int year : non_leap_years) {
      ASSERT(!Date::Validate(year, 2, 29));
    }
  }
}

void TestDate::TestAsTimestamp() {
}

void TestDate::TestFromString() {
  {
    Date expected{2000, 1, 1};
    Date actual = Date::FromString("2000-01-01");
    ASSERT_EQUAL(actual.year_, expected.year_);
    ASSERT_EQUAL(actual.month_, expected.month_);
    ASSERT_EQUAL(actual.day_, expected.day_);
  }
  {
    Date expected{2099, 12, 31};
    Date actual = Date::FromString("2099-12-31");
    ASSERT_EQUAL(actual.year_, expected.year_);
    ASSERT_EQUAL(actual.month_, expected.month_);
    ASSERT_EQUAL(actual.day_, expected.day_);
  }

  ASSERT_THROWS(([] { return Date::FromString("20000101"); }));
  ASSERT_THROWS(([] { return Date::FromString("200001-01"); }));
  ASSERT_THROWS(([] { return Date::FromString("2000-0101"); }));
}

void TestDate::TestComputeDaysDiff() {
  ASSERT_EQUAL(Date::ComputeDaysDiff({2000, 1, 1}, {2000, 1, 1}), 0);
  ASSERT_EQUAL(Date::ComputeDaysDiff({2000, 1, 2}, {2000, 1, 1}), 1);
  ASSERT_EQUAL(Date::ComputeDaysDiff({2002, 1, 1}, {2001, 1, 1}), 365);
  ASSERT_EQUAL(Date::ComputeDaysDiff({2001, 1, 1}, {2000, 1, 1}), 366);
}

void TestDate::TestAll() {
  TestRunner tr;
  RUN_TEST(tr, TestConstructor);
  RUN_TEST(tr, TestValidate);
  RUN_TEST(tr, TestAsTimestamp);
  RUN_TEST(tr, TestFromString);
  RUN_TEST(tr, TestComputeDaysDiff);
}

void TestBudgetSystem::TestEarn() {
  {
    BudgetSystem bs;
    ASSERT_EQUAL(bs.incomes_.size(), 0);
  }

  {
    BudgetSystem bs;
    Date date{2000, 1, 1};
    size_t value{1};
    bs.Earn(date, date, value);
    ASSERT_EQUAL(bs.incomes_.size(), 1);
    ASSERT_EQUAL(bs.incomes_[0], (Income{date, date, value}));
  }

  {
    BudgetSystem bs;

    Date date_1{2000, 1, 1};
    Date date_2{2000, 1, 2};
    size_t value_1{1};
    size_t value_2{2};

    bs.Earn(date_1, date_1, value_1);
    bs.Earn(date_2, date_1, value_2);

    vector<Income> expected{
      {date_1, date_1, value_1},
      {date_2, date_1, value_2},
    };

    ASSERT_EQUAL(bs.incomes_.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQUAL(bs.incomes_[i], expected[i]);
    }
  }
}

void TestBudgetSystem::TestPayTax(){
  {
    BudgetSystem bs;
    ASSERT_EQUAL(bs.incomes_.size(), 0);
    bs.PayTax({2000, 1, 1}, {2000, 1, 1});
    ASSERT_EQUAL(bs.incomes_.size(), 0);
  }

  {
    BudgetSystem bs;
    bs.Earn({2000, 1, 1}, {2000, 1, 1}, 100);
    bs.PayTax({2000, 1, 1}, {2000, 1, 1});
    ASSERT_EQUAL(bs.incomes_[0], 87);
  }

  {
    BudgetSystem bs;

    bs.Earn({2000, 1, 1}, {2000, 1, 1}, 100);
    bs.PayTax({2000, 1, 1}, {2000, 1, 1});

    bs.Earn({2000, 1, 2}, {2000, 1, 1}, 1000);
    bs.PayTax({2000, 1, 2}, {2000, 1, 1});

    ASSERT_EQUAL(bs.incomes_[0], 87);
    ASSERT_EQUAL(bs.incomes_[1], 870);
  }
}

void TestBudgetSystem::TestComputeIncome() {
  static const double EQUAL_PRECISION = 0.001;

  {
    BudgetSystem bs;

    bs.Earn({2000, 1, 1}, {2000, 1, 1}, 100);
    double income = bs.ComputeIncome({2000, 1, 1}, {2000, 1, 1});

    double expected{100};
    ASSERT(abs(income - expected) < EQUAL_PRECISION);
  }

  {
    BudgetSystem bs;

    bs.Earn({2000, 1, 1}, {2000, 1, 1}, 100);
    bs.PayTax({2000, 1, 1}, {2000, 1, 1});
    double income = bs.ComputeIncome({2000, 1, 1}, {2000, 1, 1});

    double expected{87};
    ASSERT(abs(income - expected) < EQUAL_PRECISION);
  }

  {
    BudgetSystem bs;

    bs.Earn({2000, 1, 1}, {2000, 1, 1}, 100);
    bs.Earn({2000, 1, 2}, {2000, 1, 1}, 100);
    double income = bs.ComputeIncome({2000, 1, 1}, {2000, 1, 1});

    double expected{150};
    ASSERT(abs(income - expected) < EQUAL_PRECISION);
  }

  {
    BudgetSystem bs;

    bs.Earn({2000, 1, 1}, {2000, 1, 1}, 100);
    bs.Earn({2000, 1, 2}, {2000, 1, 1}, 100);
    bs.PayTax({2000, 1, 1}, {2000, 1, 1});

    double income = bs.ComputeIncome({2000, 1, 2}, {2000, 1, 2});

    double expected{50};
    ASSERT(abs(income - expected) < EQUAL_PRECISION);
  }

  {
    BudgetSystem bs;

    bs.Earn({2000, 1, 1}, {2000, 1, 1}, 100);
    bs.PayTax({2000, 1, 1}, {2000, 1, 1});

    bs.Earn({2000, 1, 2}, {2000, 1, 1}, 100);
    bs.PayTax({2000, 1, 2}, {2000, 1, 1});

    double income_1 = bs.ComputeIncome({2000, 1, 1}, {2000, 1, 1});
    double expected_1{119.19};
    ASSERT(abs(income_1 - expected_1) < EQUAL_PRECISION);

    double income_2 = bs.ComputeIncome({2000, 1, 2}, {2000, 1, 2});
    double expected_2{43.5};
    ASSERT(abs(income_2 - expected_2) < EQUAL_PRECISION);
  }
}

void TestBudgetSystem::TestAll() {
  TestRunner tr;
  RUN_TEST(tr, TestComputeIncome);
  RUN_TEST(tr, TestEarn);
  RUN_TEST(tr, TestPayTax);
}
