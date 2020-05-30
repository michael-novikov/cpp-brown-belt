#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "test_runner.h"

using namespace std;


bool IsSubdomain(string_view subdomain, string_view domain) {
  int i = subdomain.size() - 1;
  int j = domain.size() - 1;
  while (i >= 0 && j >= 0) {
    if (subdomain[i--] != domain[j--]) {
      return false;
    }
  }
  return (j < 0 && subdomain[i] == '.')
      || (i < 0 && j < 0);
}


vector<string> ReadDomains(istream& in = cin) {
  size_t count;
  in >> count;
  in.ignore(1);

  vector<string> domains;
  for (size_t i = 0; i < count; ++i) {
    string domain;
    getline(in, domain);
    domains.push_back(domain);
  }
  return domains;
}


void TestReadDomains() {
  { // Test empty
    istringstream in("0");
    vector<string> actual = ReadDomains(in);
    vector<string> expected = {};

    ASSERT_EQUAL(actual.size(), expected.size());
    ASSERT_EQUAL(actual, expected);
  }

  { // Test one domain
    istringstream in("1\nya.ru\n");
    vector<string> actual = ReadDomains(in);
    vector<string> expected = { "ya.ru" };

    ASSERT_EQUAL(actual.size(), expected.size());
    ASSERT_EQUAL(actual, expected);
  }

  { // Test two domains
    istringstream in("2\nya.ru\ngoogle.com\n");
    vector<string> actual = ReadDomains(in);
    vector<string> expected = { "ya.ru", "google.com" };

    ASSERT_EQUAL(actual.size(), expected.size());
    ASSERT_EQUAL(actual, expected);
  }
}


void TestIsSubdomain() {
  ASSERT(IsSubdomain("", ""));

  ASSERT(IsSubdomain("ru", "ru"));
  ASSERT(IsSubdomain("ya.ru", "ya.ru"));
  ASSERT(IsSubdomain("ya.ru", "ru"));

  ASSERT(IsSubdomain("yz.abc.de", "abc.de"));
  ASSERT(IsSubdomain("wx.yz.abc.de", "abc.de"));

  ASSERT(!IsSubdomain("a.x", "b.x"));
  ASSERT(!IsSubdomain("a.x", "ab.x"));

  ASSERT(!IsSubdomain("a.x", "aa.x"));
  ASSERT(!IsSubdomain("a.x", "b.aa.x"));

  ASSERT(!IsSubdomain("abc.de", "yz.abc.de"));
  ASSERT(!IsSubdomain("abc.de", "wx.yz.abc.de"));
}


void RunTests() {
  TestRunner tr;
  RUN_TEST(tr, TestReadDomains);
  RUN_TEST(tr, TestIsSubdomain);
}


int main() {
  RunTests();

  // const vector<string> banned_domains = ReadDomains();
  // const vector<string> domains_to_check = ReadDomains();

  // for (string_view domain : banned_domains) {
  //   reverse(begin(domain), end(domain));
  // }
  // sort(begin(banned_domains), end(banned_domains));

  // size_t insert_pos = 0;
  // for (string& domain : banned_domains) {
  //   if (insert_pos == 0 || !IsSubdomain(domain, banned_domains[insert_pos - 1])) {
  //     swap(banned_domains[insert_pos++], domain);
  //   }
  // }
  // banned_domains.resize(insert_pos);

  // for (const string_view domain : domains_to_check) {
  //   if (const auto it = upper_bound(begin(banned_domains), end(banned_domains), domain);
  //       it != begin(banned_domains) && IsSubdomain(domain, *prev(it))) {
  //     cout << "Good" << endl;
  //   } else {
  //     cout << "Bad" << endl;
  //   }
  // }
  return 0;
}
