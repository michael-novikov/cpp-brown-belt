#pragma once

#include "stop_manager.h"
#include "bus.h"

#include <string_view>
#include <vector>
#include <unordered_map>
#include <memory>

class TransportManager {
public:
  using RouteNumber = BusRoute::RouteNumber;

  void AddStop(const std::string& name, double latitude, double longitude, const std::unordered_map<std::string, double>& distances);
  void AddBus(const RouteNumber& route_number, const std::vector<std::string>& stop_names, bool cyclic);

  std::pair<double, double> ComputeBusRouteLength(const RouteNumber& route_number);
  std::string GetStopInfo(const std::string& stop_name);
  std::string GetBusInfo(const RouteNumber& route_number);
  void ProcessQuery(std::string_view query);
private:
  std::unordered_map<std::string, Stop> stops_;
  std::unordered_map<std::string, std::unordered_map<std::string, double>> distances_;
  std::unordered_map<RouteNumber, BusRoute> buses_;
};

