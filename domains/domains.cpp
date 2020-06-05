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

bool IsSubdomainReversed(string_view subdomain, string_view domain) {
  int i = 0;
  int j = 0;
  while (i < subdomain.size() && j < domain.size()) {
    if (subdomain[i++] != domain[j++]) {
      return false;
    }
  }
  return (j == domain.size()) && (i == j || subdomain[i] == '.');
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

bool IsBannedDomain(string_view domain, const vector<string>& banned_domains) {
  const auto it = upper_bound(begin(banned_domains), end(banned_domains), domain);
  return it != begin(banned_domains) && IsSubdomainReversed(domain, *prev(it));
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

void TestIsSubdomainReversed() {
  vector<pair<string, string>> positive_tests = {
    { "", "" },
    { "ur", "ur" },
    { "ur.ay", "ur.ay" },
    { "ur.ay", "ur" },
    { "ed.cba.zy", "ed.cba" },
    { "ed.cba.zy.xw", "ed.cba" },
  };

  vector<pair<string, string>> negative_tests = {
    { "x.a", "x.b" },
    { "x.a", "x.ba" },
    { "x.a", "x.aa" },
    { "x.a", "x.aa.b" },
    { "ed.cba", "ed.cba.zy" },
    { "ed.cba", "ed.abc.zy.xw" },
  };

  for (const auto& [subdomain, domain] : positive_tests) {
    ASSERT(IsSubdomainReversed(subdomain, domain));
  }

  for (const auto& [subdomain, domain] : negative_tests) {
    ASSERT(!IsSubdomainReversed(subdomain, domain));
  }
}

void TestIsBannedDomain() {
  // TODO: remove any algorithms from unit test;
  const vector<string> banned_domains = [] {
    vector<string> tmp = { "ya.ru", "maps.me", "m.ya.ru", "com" };
    for (string& domain : tmp) {
      reverse(begin(domain), end(domain));
    }
    sort(begin(tmp), end(tmp));
    return tmp;
  }();

  vector<string> positive_tests = {"ya.ru", "ya.com", "m.maps.me", "moscow.m.ya.ru", "maps.com" };
  vector<string> negative_tests = {"maps.ru", "ya.ya"};

  for (auto domain : positive_tests) {
    reverse(begin(domain), end(domain));
    ASSERT(IsBannedDomain(domain, banned_domains));
  }

  for (auto domain : negative_tests) {
    reverse(begin(domain), end(domain));
    ASSERT(!IsBannedDomain(domain, banned_domains));
  }
}

void RunTests() {
  TestRunner tr;
  RUN_TEST(tr, TestReadDomains);
  RUN_TEST(tr, TestIsSubdomain);
  RUN_TEST(tr, TestIsSubdomainReversed);
  RUN_TEST(tr, TestIsBannedDomain);
}

int main() {
  RunTests();

  const vector<string> banned_domains = [] {
    vector<string> input = ReadDomains();

    for (string& domain : input) {
      reverse(begin(domain), end(domain));
    }

    sort(begin(input), end(input));
    return input;
  }();

  const vector<string> domains_to_check = [] {
    vector<string> input = ReadDomains();

    for (string& domain : input) {
      reverse(begin(domain), end(domain));
    }

    return input;
  }();

  for (const string_view domain : domains_to_check) {
    if (IsBannedDomain(domain, banned_domains)) {
      cout << "Bad" << endl;
    } else {
      cout << "Good" << endl;
    }
  }

  return 0;
}
