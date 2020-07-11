#include "domains.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

using namespace std;

int main() {
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

