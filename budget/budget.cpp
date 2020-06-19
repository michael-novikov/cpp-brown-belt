#include <iostream>
#include <string>
#include <string_view>

#include "date.h"
#include "budget_system.h"

#ifdef BUDGET_TEST
#include "budget_test.h"
#endif

using namespace std;

enum class QueryType {
  COMPUTE_INCOME,
  EARN,
  PAY_TAX,

  NOT_SUPPORTED
};

QueryType ParseQueryType(string_view query) {
  if (query == "Earn") {
    return QueryType::EARN;
  } else if (query == "ComputeIncome") {
    return QueryType::COMPUTE_INCOME;
  } else if (query == "PayTax") {
    return QueryType::PAY_TAX;
  } else {
    return QueryType::NOT_SUPPORTED;
  }
}

int main() {
#ifdef BUDGET_TEST
  TestDate::TestAll();
  TestBudgetSystem::TestAll();
#endif

  cout.precision(25);

  BudgetSystem budget_system;

  size_t num_commands;
  cin >> num_commands;

  for (size_t i = 0; i < num_commands; ++i) {
    string query_type;
    cin >> query_type;

    string from_str;
    cin >> from_str;
    auto from = Date::FromString(from_str);

    string to_str;
    cin >> to_str;
    auto to = Date::FromString(to_str);

    switch (ParseQueryType(query_type)) {
      case QueryType::COMPUTE_INCOME:
        cout << budget_system.ComputeIncome(from, to) << endl;
        break;
      case QueryType::EARN:
        IncomeValue income;
        cin >> income;
        budget_system.Earn(from, to, income);
        break;
      case QueryType::PAY_TAX:
        budget_system.PayTax(from, to);
        break;
      case QueryType::NOT_SUPPORTED:
        cout << "Operation is not supported" << endl;
        break;
    }
  }
}
