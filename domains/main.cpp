#include "domains.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

using namespace std;

int main() {
  vector<string> banned_domains = ReadDomains();

  for (string& domain : banned_domains) {
    reverse(begin(domain), end(domain));
  }
  sort(begin(banned_domains), end(banned_domains));

  size_t insert_pos = 0;
  for (string& domain : banned_domains) {
    if (insert_pos == 0 || !IsSubdomainReversed(domain, banned_domains[insert_pos - 1])) {
      swap(banned_domains[insert_pos++], domain);
    }
  }
  banned_domains.resize(insert_pos);

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

