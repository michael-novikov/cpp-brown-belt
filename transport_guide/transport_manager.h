#pragma once

#include "stop_manager.h"
#include "bus.h"
#include "transport_manager_command.h"

#include <string_view>
#include <vector>
#include <unordered_map>
#include <memory>

class TransportManager {
public:
  using RouteNumber = BusRoute::RouteNumber;

  void AddStop(const std::string& name, double latitude, double longitude, const std::unordered_map<std::string, double>& distances);
  void AddBus(const RouteNumber& route_number, const std::vector<std::string>& stop_names, bool cyclic);

  std::pair<size_t, double> ComputeBusRouteLength(const RouteNumber& route_number);
  StopInfo GetStopInfo(const std::string& stop_name, size_t request_id);
  BusInfo GetBusInfo(const RouteNumber& route_number, size_t request_id);
  void ProcessQuery(std::string_view query);
private:
  std::unordered_map<std::string, Stop> stops_;
  std::unordered_map<std::string, std::unordered_map<std::string, size_t>> distances_;
  std::unordered_map<RouteNumber, BusRoute> buses_;
};

