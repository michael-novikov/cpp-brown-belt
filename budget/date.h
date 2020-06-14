#ifndef DATE_H
#define DATE_H

#include <ctime>
#include <string>

#ifdef BUDGET_TEST
#include "budget_test.h"
#endif

class Date {
public:
  Date(int year, int month, int day);

  time_t AsTimestamp() const;

  static bool Validate(int year, int month, int day);
  static Date FromString(std::string str);
  static int ComputeDaysDiff(const Date& date_to, const Date& date_from);

  bool operator==(const Date& other);
  bool operator<(const Date& other);
  bool operator<=(const Date& other);

#ifdef BUDGET_TEST
  friend class TestDate;
#endif
private:
  static const std::string delim;

  int day_;
  int month_;
  int year_;
};

#endif // DATE_H
