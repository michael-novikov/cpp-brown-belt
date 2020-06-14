#include "budget_system.h"

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>

bool Income::operator==(const Income& other) const {
  return from == other.from
    && to == other.to
    && value == other.value;
}

PureIncome BudgetSystem::ComputeIncome(const Date& from, const Date& to) const {
  // TODO: implement me
  throw std::runtime_error("Not implemented "
      + std::to_string(from.Day()) + " " + std::to_string(to.Day()));
}

void BudgetSystem::Earn(const Date& from, const Date& to, IncomeValue value) {
  IncomeValue income_after = incomes_.lower_bound(to)->second;
  Date date_after{to.Year(), to.Month(), to.Day() + 1};
  auto [end, end_inserted] = incomes_.emplace(date_after, income_after);

  auto [begin, begin_inserted] = incomes_.emplace(from, IncomeValue{0});
  double value_per_day =
      static_cast<double>(value) / (Date::ComputeDaysDiff(end->first, begin->first));
  for (auto it = begin; it != end; ++it) {
    it->second += value_per_day;
  }
}

void BudgetSystem::PayTax(const Date& from, const Date& to) {
  // TODO: implement me
  throw std::runtime_error("Not implemented "
      + std::to_string(from.Day()) + " " + std::to_string(to.Day()));
}
