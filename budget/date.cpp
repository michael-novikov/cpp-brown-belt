#include "date.h"

#include <sstream>

using namespace std;

int Date::ComputeDaysDiff(const Date& date_to, const Date& date_from) {
  const time_t timestamp_to = date_to.AsTimestamp();
  const time_t timestamp_from = date_from.AsTimestamp();
  static const int SECONDS_IN_DAY = 60 * 60 * 24;
  return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

Date::Date(int year, int month, int day) {
  if (!Validate(year, month, day)) {
    const string bad_date = (
        ostringstream{} << to_string(year) << delim << to_string(month) << delim << to_string(day)
    ).str();
    throw std::domain_error("Wrong date format: " + bad_date);
  }
  year_ = year;
  month_ = month;
  day_ = day;
}

time_t Date::AsTimestamp() const {
  std::tm t;
  t.tm_sec   = 0;
  t.tm_min   = 0;
  t.tm_hour  = 0;
  t.tm_mday  = day_;
  t.tm_mon   = month_ - 1;
  t.tm_year  = year_ - 1900;
  t.tm_isdst = 0;
  return mktime(&t);
}

bool Date::Validate(int year, int month, int day) {
  if (month == 2 && day == 29) {
    return year % 4 == 0;
  }

  return year >= 2000 && year <= 2099
    && month >= 1 && month <= 12
    && day >= 1 && day <= 31;
}

Date Date::FromString(std::string str) {
  istringstream in{str};
  int year, month, day;

  in >> year;
  in.ignore(1);
  in >> month;
  in.ignore(1);
  in >> day;

  return Date{year, month, day};
}

const std::string Date::delim = "-";

