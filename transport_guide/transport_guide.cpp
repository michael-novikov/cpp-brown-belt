#include "bus.h"
#include "transport_manager.h"

#include "stop_manager.h"
#include "transport_manager_command.h"

#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string_view>

using namespace std;

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter = " ") {
  const size_t pos = s.find(delimiter);
  if (pos == s.npos) {
    return {s, nullopt};
  } else {
    return {s.substr(0, pos), s.substr(pos + delimiter.length())};
  }
}

pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter = " ") {
  const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
  return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view& s, string_view delimiter = " ") {
  const auto [lhs, rhs] = SplitTwo(s, delimiter);
  s = rhs;
  return lhs;
}

int ConvertToInt(string_view str) {
  // use std::from_chars when available to git rid of string copy
  size_t pos;
  const int result = stoi(string(str), &pos);
  if (pos != str.length()) {
    std::stringstream error;
    error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
    throw invalid_argument(error.str());
  }
  return result;
}

double ConvertToDouble(string_view str) {
  // use std::from_chars when available to git rid of string copy
  size_t pos;
  const double result = stod(string(str), &pos);
  if (pos != str.length()) {
    std::stringstream error;
    error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
    throw invalid_argument(error.str());
  }
  return result;
}

void HandleInputCommand(TransportManager& manager, string_view command) {
  auto [type, body] = SplitTwo(command, " ");
  if (type == "Stop") {
    auto stop_name = ReadToken(body, ": ");

    auto latitude_str = ReadToken(body, ", ");
    double latitude = ConvertToDouble(latitude_str);

    auto longitude_str = ReadToken(body, ", ");
    double longitude = ConvertToDouble(longitude_str);

    std::unordered_map<std::string, double> distances;

    while (body != "") {
      auto distance_description = ReadToken(body, ", ");
      auto [distance_str, stop_to] = SplitTwo(distance_description, "m to ");
      auto distance = ConvertToInt(distance_str);
      distances[string{stop_to}] = static_cast<double>(distance);
    }

    manager.AddStop(string{stop_name}, latitude, longitude, distances);
  }
  else if (type == "Bus") {
    auto [route_number_str, description] = SplitTwo(body, ": ");
    vector<string> stops;
    auto [first, last] = SplitTwoStrict(description, " > ");
    auto [delimiter, cyclic] = last.has_value() ? pair{" > ", true} : pair{" - ", false};
    auto stop_name = static_cast<string>(ReadToken(description, delimiter));
    while (stop_name != "") {
      stops.push_back(stop_name);
      stop_name = static_cast<string>(ReadToken(description, delimiter));
    }
    manager.AddBus(string{route_number_str}, stops, cyclic);
  }
  else {
    throw std::invalid_argument("Unsupported command");
  }
}

void HandleOutputCommand(TransportManager& manager, string_view command) {
  auto [type, body] = SplitTwo(command, " ");
  if (type == "Stop") {
    string stop_name{body};
    cout << manager.GetStopInfo(stop_name) << endl;
  }
  else if (type == "Bus") {
    string route_number{body};
    cout << manager.GetBusInfo(route_number) << endl;
  }
  else {
    throw std::invalid_argument("Unsupported command");
  }
}

int main() {
  TransportManager manager;

  size_t fill_query_number;
  cin >> fill_query_number;
  cin.ignore();
  for (size_t i = 0; i < fill_query_number; ++i) {
    string line;
    getline(cin, line);
    HandleInputCommand(manager, line);
  }

  size_t query_number;
  cin >> query_number;
  cin.ignore();
  for (size_t i = 0; i < query_number; ++i) {
    string line;
    getline(cin, line);
    HandleOutputCommand(manager, line);
  }
}
