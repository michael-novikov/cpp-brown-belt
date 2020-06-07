#include "domains.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

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


vector<string> ReadDomains(istream& in) {
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

