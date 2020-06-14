#ifndef BUDGET_H
#define BUDGET_H

#include <vector>
#include <utility>

#include "date.h"

#ifdef BUDGET_TEST
#include "budget_test.h"
#endif

using IncomeValue = size_t;
using PureIncome = double;

struct Income {
  Date from;
  Date to;
  IncomeValue value;

  bool operator==(const Income& other);
};

class BudgetSystem {
public:
  BudgetSystem() = default;

  PureIncome ComputeIncome(const Date& from, const Date& to) const;
  void Earn(const Date& from, const Date& to, IncomeValue value);
  void PayTax(const Date& from, const Date& to);

#ifdef BUDGET_TEST
  friend class TestBudgetSystem;
#endif
private:
  std::vector<Income> incomes_;
};

#endif // BUDGET_H
