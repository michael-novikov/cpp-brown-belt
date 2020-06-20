#ifndef BUDGET_H
#define BUDGET_H

#include <map>
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

  bool operator==(const Income& other) const;
};

class BudgetSystem {
public:
  BudgetSystem() = default;

  PureIncome ComputeIncome(const Date& from, const Date& to) ;
  void Earn(const Date& from, const Date& to, IncomeValue value);
  void PayTax(const Date& from, const Date& to);

  static PureIncome ComputeIncomeAfterTax(PureIncome income);

#ifdef BUDGET_TEST
  friend class TestBudgetSystem;
#endif
private:
  std::map<Date, PureIncome> incomes_ = { { Date{2000, 1, 1}, PureIncome{0} }, { Date{2100, 1, 1}, PureIncome{0} } };

  decltype(incomes_)::iterator AddBoundDate(const Date& date);
  PureIncome ComputeFromDateToBound(const Date& date);
  PureIncome ComputeFromBoundToDate(const Date& date);
};

#endif // BUDGET_H
