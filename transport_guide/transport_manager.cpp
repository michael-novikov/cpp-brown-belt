#include "transport_manager.h"
#include "bus.h"
#include "stop_manager.h"

#include <iterator>
#include <sstream>
#include <set>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace std;

void TransportManager::AddStop(const string& name, double latitude, double longitude, const unordered_map<string, double>& distances) {
  if (stops_.count(name)) {
    return;
  }

  stops_[name] = Stop{name, Coordinates{latitude, longitude}};

  for (const auto& [stop_name, dist] : distances) {
    distances_[name][stop_name] = dist;
    if (!distances_[stop_name].count(name)) {
      distances_[stop_name][name] = dist;
    }
  }
}

void TransportManager::AddBus(const RouteNumber& bus_no, const std::vector<std::string>& stop_names, bool cyclic) {
  buses_[string{bus_no}] = cyclic ? BusRoute::CreateCyclicBusRoute(bus_no, stop_names)
    : BusRoute::CreateRawBusRoute(bus_no, stop_names);
}

std::pair<size_t, double> TransportManager::ComputeBusRouteLength(const RouteNumber& route_number) {
  if (!buses_.count(route_number)) {
    return {0, 0};
  }

  if (auto route_length = buses_[route_number].RouteLength(); route_length.has_value()) {
    return route_length.value();
  }

  size_t distance_road{0};
  double distance_direct{0.0};
  vector<const Stop*> bus_stops;
  for (const auto& stop_name : buses_[route_number].Stops()) {
    if (stops_.count(stop_name)) {
      bus_stops.push_back(&stops_[stop_name]);
    }
  }

  for (size_t i = 0; i < bus_stops.size() - 1; ++i) {
    distance_direct += Coordinates::Distance(bus_stops[i]->StopCoordinates(),
                                             bus_stops[i + 1]->StopCoordinates());
    distance_road += distances_[bus_stops[i]->Name()][bus_stops[i + 1]->Name()];
  }

  buses_[route_number].SetRouteLength(distance_road, distance_direct);
  return {distance_road, distance_direct};
}

StopInfo TransportManager::GetStopInfo(const string& stop_name, size_t request_id) {
  if (!stops_.count(stop_name)) {
    return StopInfo{
      .request_id = request_id,
      .error_message = "not found",
    };
  }

  set<RouteNumber> buses_with_stop;
  for (const auto& bus : buses_) {
    if (bus.second.ContainsStop(stop_name)) {
        buses_with_stop.insert(bus.first);
    }
  }

  return StopInfo{
    .buses = vector<string>{begin(buses_with_stop), end(buses_with_stop)},
    .request_id = request_id,
  };
}

BusInfo TransportManager::GetBusInfo(const RouteNumber& bus_no, size_t request_id) {
  if (!buses_.count(bus_no)) {
    return BusInfo{
      .request_id = request_id,
      .error_message = "not found",
    };
  }

  const auto& bus = buses_.at(bus_no);
  const auto [road_length, direct_length] = ComputeBusRouteLength(bus_no);

  return BusInfo {
    .route_length = road_length,
    .request_id = request_id,
    .curvature = road_length / direct_length,
    .stop_count = bus.Stops().size(),
    .unique_stop_count = bus.UniqueStopNumber(),
  };
}
