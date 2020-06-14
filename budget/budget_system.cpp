#include "budget_system.h"

#include <iostream>

bool Income::operator==(const Income& other){
  return from == other.from
    && to == other.to
    && value == other.value;
}

PureIncome BudgetSystem::ComputeIncome(const Date& from, const Date& to) const {
}

void BudgetSystem::Earn(const Date& from, const Date& to, IncomeValue value) {
}

void BudgetSystem::PayTax(const Date& from, const Date& to) {
}
