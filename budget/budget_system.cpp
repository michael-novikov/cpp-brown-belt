#include "budget_system.h"

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <iterator>

#include "date.h"

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
  auto nearest = incomes_.lower_bound(from);
  auto [income_begin, begin_inserted] = incomes_.emplace(from, nearest->second);
  auto [income_end, end_inserted] = incomes_.emplace(Date::Next(to), PureIncome{0});

  PureIncome value_per_day{
    static_cast<PureIncome>(value)
    / Date::ComputeDaysDiff(income_end->first, income_begin->first)
  };

  for (auto it = std::next(income_begin); it != std::next(income_end); ++it) {
    it->second += value_per_day;
  }
}

void BudgetSystem::PayTax(const Date& from, const Date& to) {
  auto before_to = incomes_.upper_bound(to)->first;
  if (before_to < from) {
    return;
  }

  auto [tax_begin, begin_inserted] = incomes_.emplace(from, incomes_.lower_bound(from)->second);
  auto [tax_end, end_inserted] = incomes_.emplace(Date::Next(to), incomes_.lower_bound(to)->second);

  static const double tax_multiplier = (1.0 - 0.13);
  for (auto it = tax_begin; it != tax_end; ++it) {
    it->second *= tax_multiplier;
  }
}
