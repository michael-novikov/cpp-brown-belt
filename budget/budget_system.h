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

  PureIncome ComputeIncome(const Date& from, const Date& to) const;
  void Earn(const Date& from, const Date& to, IncomeValue value);
  void Spend(const Date& from, const Date& to, IncomeValue value);
  void PayTax(const Date& from, const Date& to, size_t percent = 13);

  static PureIncome ComputeIncomeAfterTax(PureIncome income, size_t percent = 13);

#ifdef BUDGET_TEST
  friend class TestBudgetSystem;
#endif
private:
  std::map<Date, PureIncome> incomes_ = { { Date{2000, 1, 1}, PureIncome{0} }, { Date{2100, 1, 1}, PureIncome{0} } };
  std::map<Date, PureIncome> spendings_ = { { Date{2000, 1, 1}, PureIncome{0} }, { Date{2100, 1, 1}, PureIncome{0} } };

  enum class Transaction {
    EARNING,
    SPENDING,
  };

  void UpdateTransactions(const Date& from, const Date& to, IncomeValue value, Transaction tr);

  std::map<Date, PureIncome>::iterator AddBoundDate(std::map<Date, PureIncome>& transaction_history, const Date& date);

  PureIncome ComputeFromDateToBound(const std::map<Date, PureIncome>& transaction_history, const Date& date) const;
};

#endif // BUDGET_H
