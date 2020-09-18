#include "json_parser.h"

#include "json.h"

using namespace std;
using namespace Json;

namespace JsonArgs {

unique_ptr<InCommand> ReadInputCommand(const Node& node) {
  auto command = node.AsMap();
  auto type = command["type"].AsString();
  if (type == "Stop") {
    auto stop_name = command["name"].AsString();

    const auto& latitude_node = command["latitude"];
    double latitude = holds_alternative<int>(latitude_node) ? latitude_node.AsInt() : latitude_node.AsDouble();

    const auto& longitude_node = command["longitude"];
    double longitude = holds_alternative<int>(longitude_node) ? longitude_node.AsInt() : longitude_node.AsDouble();

    std::unordered_map<std::string, double> distances;
    auto distances_nodes = command["road_distances"].AsMap();
    for (const auto& [to, road_length] : distances_nodes) {
      distances[to] = static_cast<double>(road_length.AsInt());
    }

    return make_unique<NewStopCommand>(string{stop_name}, latitude, longitude,
                                       distances);
  } else if (type == "Bus") {
    auto route_number = command["name"].AsString();

    vector<string> stops;
    auto stop_nodes = command["stops"].AsArray();
    transform(begin(stop_nodes), end(stop_nodes),
              back_inserter(stops),
              [](const Node& n) { return n.AsString(); });
    auto is_roundtrip = command["is_roundtrip"].AsBool();
    return make_unique<NewBusCommand>(route_number, stops, is_roundtrip);
  } else {
    throw std::invalid_argument("Unsupported command");
  }
}

unique_ptr<OutCommand> ReadOutputCommand(const Node& node) {
  auto command = node.AsMap();
  auto type = command["type"].AsString();
  auto request_id = static_cast<size_t>(command["id"].AsInt());
  if (type == "Stop") {
    auto stop_name = command["name"].AsString();
    return make_unique<StopDescriptionCommand>(stop_name, request_id);
  } else if (type == "Bus") {
    auto route_number = command["name"].AsString();
    return make_unique<BusDescriptionCommand>(route_number, request_id);
  } else {
    throw std::invalid_argument("Unsupported command");
  }
}

TransportManagerCommands ReadCommands(std::istream& s) {
  TransportManagerCommands commands;

  auto root = Load(s).GetRoot().AsMap();

  auto base_requests = root["base_requests"].AsArray();
  for (const auto& node : base_requests) {
    commands.input_commands.push_back(ReadInputCommand(node));
  }

  auto stat_requests = root["stat_requests"].AsArray();
  for (const auto& node : stat_requests) {
    commands.output_commands.push_back(ReadOutputCommand(node));
  }

  return commands;
}

void PrintResults(const std::vector<StopInfo>& stop_info, const std::vector<BusInfo>& bus_info, std::ostream& output) {
  vector<Node> result;

  for (const auto& bus : bus_info) {
    map<string, Node> bus_dict = {
      {"request_id", Node(static_cast<int>(bus.request_id))},
    };

    if (bus.error_message.has_value()) {
      bus_dict["error_message"] = Node(bus.error_message.value());
    }
    else {
      bus_dict["route_length"] = Node(static_cast<int>(bus.route_length));
      bus_dict["curvature"] = Node(bus.curvature);
      bus_dict["stop_count"] = Node(static_cast<int>(bus.stop_count));
      bus_dict["unique_stop_count"] = Node(static_cast<int>(bus.unique_stop_count));
    }
    result.push_back(Node(move(bus_dict)));
  }

  for (const auto& stop : stop_info) {
    map<string, Node> stop_dict = {
      {"request_id", Node(static_cast<int>(stop.request_id))},
    };

    if (stop.error_message.has_value()) {
      stop_dict["error_message"] = Node(stop.error_message.value());
    }
    else {
      vector<Node> buses;
      transform(begin(stop.buses), end(stop.buses),
                back_inserter(buses),
                [](const string& route_number) { return Node(route_number); });

      stop_dict["buses"] = Node(buses);
    }
    result.push_back(Node(move(stop_dict)));
  }

  Node root{result};
  Document doc{root};

  Print(doc, output);
}

} // namespace JsonArgs
