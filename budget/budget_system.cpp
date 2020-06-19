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
  PureIncome res{0};
  for (auto cur = from; cur <= to; ) {
    const auto& [bound, income] = *incomes_.upper_bound(cur);
    auto next = std::min(bound, Date::Next(to));
    res += income * Date::ComputeDaysDiff(next, cur);
    cur = next;
  }
  return res;
}

void BudgetSystem::Earn(const Date& from, const Date& to, IncomeValue value) {
  if (Date::ComputeDaysDiff(to, from) < 0) {
    return;
  }

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

PureIncome BudgetSystem::ComputeIncomeAfterTax(PureIncome income) {
  return 0.87 * income;
}

void BudgetSystem::PayTax(const Date& from, const Date& to) {
  if (Date::ComputeDaysDiff(to, from) < 0) {
    return;
  }

  auto [before_tax, before_tax_inserted] =
    incomes_.emplace(from, incomes_.lower_bound(from)->second);

  auto day_after = Date::Next(to);
  auto [last_payment, end_inserted] =
    incomes_.emplace(day_after, incomes_.lower_bound(day_after)->second);

  auto tax_begin = next(before_tax);
  auto tax_end = next(last_payment);
  for (auto it = tax_begin; it != tax_end; ++it) {
    it->second = ComputeIncomeAfterTax(it->second);
  }
}
