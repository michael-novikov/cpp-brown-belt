#ifndef DOMAINS_H
#define DOMAINS_H

#include <iostream>
#include <vector>
#include <string>
#include <string_view>

bool IsSubdomain(std::string_view subdomain, std::string_view domain);
bool IsSubdomainReversed(std::string_view subdomain, std::string_view domain);
std::vector<std::string> ReadDomains(std::istream& in = std::cin);
bool IsBannedDomain(std::string_view domain, const std::vector<std::string>& banned_domains);

#endif // DOMAINS_H
