#include "budget_system.h"

#include <cstddef>
#include <iostream>
#include <numeric>
#include <ostream>
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
  auto accumulate_income = [this](const std::map<Date, PureIncome>& transaction_history, const Date& from, const Date& to) -> PureIncome {
    PureIncome res{0};
    const auto begin_ = next(transaction_history.upper_bound(from));
    const auto end_ = next(transaction_history.upper_bound(Date::Next(to)));

    res += ComputeFromDateToBound(transaction_history, from);
    res += std::accumulate(begin_, end_, PureIncome{0}, [](PureIncome sum, auto p) { return sum + p.second; });
    res -= ComputeFromDateToBound(transaction_history, Date::Next(to));

    return res;
  };

  return accumulate_income(incomes_, from, to) - accumulate_income(spendings_, from, to);
}

PureIncome BudgetSystem::ComputeFromDateToBound(const std::map<Date, PureIncome>& transaction_history, const Date& date) const {
  if (date == std::prev(transaction_history.end())->first) {
    return PureIncome{0};
  }

  auto next = transaction_history.upper_bound(date);
  auto previous = std::prev(next);

  int days_original = Date::ComputeDaysDiff(next->first, previous->first);
  int days_after = Date::ComputeDaysDiff(next->first, date);

  return next->second * days_after / days_original;
}

std::map<Date, PureIncome>::iterator BudgetSystem::AddBoundDate(std::map<Date, PureIncome>& transaction_history, const Date& date) {
  if (auto it = transaction_history.find(date); it != std::end(transaction_history)) {
    return it;
  }

  auto nearest = transaction_history.upper_bound(date);
  auto previous = std::prev(nearest);

  int days_original = Date::ComputeDaysDiff(nearest->first, previous->first);
  int days_before = Date::ComputeDaysDiff(date, previous->first);
  int days_after = Date::ComputeDaysDiff(nearest->first, date);

  PureIncome value = nearest->second * days_before / days_original;
  nearest->second = nearest->second * days_after / days_original;

  return transaction_history.emplace(date, value).first;
}

void BudgetSystem::Earn(const Date& from, const Date& to, IncomeValue value) {
  if (Date::ComputeDaysDiff(to, from) < 0) {
    return;
  }

  return UpdateTransactions(from, to, value, Transaction::EARNING);
}

void BudgetSystem::Spend(const Date& from, const Date& to, IncomeValue value) {
  if (Date::ComputeDaysDiff(to, from) < 0) {
    return;
  }

  return UpdateTransactions(from, to, value, Transaction::SPENDING);
}

void BudgetSystem::UpdateTransactions(const Date& from, const Date& to, IncomeValue value, Transaction tr) {
  auto* transaction_history = &incomes_;
  if (tr == Transaction::SPENDING) {
    transaction_history = &spendings_;
  }

  auto begin_ = AddBoundDate(*transaction_history, from);
  auto end_ = AddBoundDate(*transaction_history, Date::Next(to));

  const int days = Date::ComputeDaysDiff(end_->first, begin_->first);
  auto cur = from;
  for (auto it = std::next(begin_); it != std::next(end_); ++it) {
    int days_in_interval = Date::ComputeDaysDiff(it->first, cur);
    it->second += (static_cast<PureIncome>(value) * days_in_interval) / days;
    cur = it->first;
  }
}

PureIncome BudgetSystem::ComputeIncomeAfterTax(PureIncome income, size_t percent) {
  if (percent > 100) {
    throw std::out_of_range("Percent must be less or equal than 100");
  }

  return ((100 - percent) / double{100}) * income;
}

void BudgetSystem::PayTax(const Date& from, const Date& to, size_t percent) {
  if (Date::ComputeDaysDiff(to, from) < 0) {
    return;
  }

  auto before_tax = AddBoundDate(incomes_, from);
  auto last_payment = AddBoundDate(incomes_, Date::Next(to));

  auto tax_begin = next(before_tax);
  auto tax_end = next(last_payment);
  for (auto it = tax_begin; it != tax_end; ++it) {
    it->second = ComputeIncomeAfterTax(it->second, percent);
  }
}
