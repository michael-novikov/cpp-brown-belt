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

  res += ComputeFromDateToBound(from);

  const auto begin_ = ++incomes_.upper_bound(from);
  const auto end_ = incomes_.upper_bound(Date::Next(to));
  for (auto it = begin_; it != next(end_); ++it) {
    res += it->second;
  }

  res -= ComputeFromDateToBound(Date::Next(to));

  return res;
}

PureIncome BudgetSystem::ComputeFromDateToBound(const Date& date) const {
  if (date == std::prev(incomes_.end())->first) {
    return PureIncome{0};
  }

  auto next = incomes_.upper_bound(date);
  auto previous = std::prev(next);

  int days_original = Date::ComputeDaysDiff(next->first, previous->first);
  int days_after = Date::ComputeDaysDiff(next->first, date);

  return next->second * days_after / days_original;
}

PureIncome BudgetSystem::ComputeFromBoundToDate(const Date& date) const {
  auto next = incomes_.upper_bound(date);
  auto previous = std::prev(next);

  int days_original = Date::ComputeDaysDiff(next->first, previous->first);
  int days_before = Date::ComputeDaysDiff(date, previous->first);

  return next->second * days_before / days_original;
}

decltype(BudgetSystem::incomes_)::iterator BudgetSystem::AddBoundDate(const Date& date) {
  if (auto it = incomes_.find(date); it != std::end(incomes_)) {
    return it;
  }

  auto nearest = incomes_.upper_bound(date);
  auto previous = std::prev(nearest);

  int days_original = Date::ComputeDaysDiff(nearest->first, previous->first);
  int days_before = Date::ComputeDaysDiff(date, previous->first);
  int days_after = Date::ComputeDaysDiff(nearest->first, date);

  PureIncome value = nearest->second * days_before / days_original;
  nearest->second = nearest->second * days_after / days_original;

  return incomes_.emplace(date, value).first;
}

void BudgetSystem::Earn(const Date& from, const Date& to, IncomeValue value) {
  if (Date::ComputeDaysDiff(to, from) < 0) {
    return;
  }

  auto income_begin = AddBoundDate(from);
  auto income_end = AddBoundDate(Date::Next(to));

  const int days = Date::ComputeDaysDiff(income_end->first, income_begin->first);
  auto cur = from;
  for (auto it = std::next(income_begin); it != std::next(income_end); ++it) {
    int days_in_interval = Date::ComputeDaysDiff(it->first, cur);
    it->second += (static_cast<PureIncome>(value) * days_in_interval) / days;
    cur = it->first;
  }
}

PureIncome BudgetSystem::ComputeIncomeAfterTax(PureIncome income) {
  return 0.87 * income;
}

void BudgetSystem::PayTax(const Date& from, const Date& to) {
  if (Date::ComputeDaysDiff(to, from) < 0) {
    return;
  }

  auto before_tax = AddBoundDate(from);
  auto last_payment = AddBoundDate(Date::Next(to));

  auto tax_begin = next(before_tax);
  auto tax_end = next(last_payment);
  for (auto it = tax_begin; it != tax_end; ++it) {
    it->second = ComputeIncomeAfterTax(it->second);
  }
}
