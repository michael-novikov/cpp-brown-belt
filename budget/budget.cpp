#include <iostream>

#ifdef BUDGET_TEST
#include "budget_test.h"
#endif

using namespace std;

//enum class QueryType {
//  COMPUTE_INCOME,
//  EARN,
//  PAY_TAX,
//};

int main() {
  TestDate::TestAll();
}
