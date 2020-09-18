#pragma once

#include <algorithm>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>
#include <limits>
#include <optional>
#include <memory>

enum class InCommandType {
  NEW_STOP,
  NEW_BUS,

  NUM_COMMANDS,
};

enum class OutCommandType {
  STOP_DESCRIPTION,
  BUS_DESCRIPTION,

  NUM_COMMANDS,
};

struct InCommand {
public:
  InCommand(InCommandType type) : type_(type) {}
  virtual InCommandType Type() const { return type_; }

private:
  InCommandType type_{InCommandType::NUM_COMMANDS};
};

struct OutCommand {
public:
  OutCommand(OutCommandType type) : type_(type) {}
  virtual OutCommandType Type() const { return type_; }

private:
  OutCommandType type_{OutCommandType::NUM_COMMANDS};
};

struct TransportManagerCommands {
  std::vector<std::unique_ptr<InCommand>> input_commands;
  std::vector<std::unique_ptr<OutCommand>> output_commands;
};

struct NewStopCommand : public InCommand {
public: 
  NewStopCommand(std::string name, double latitude, double longitude, std::unordered_map<std::string, double> distances)
    : InCommand(InCommandType::NEW_STOP)
    , name_(move(name))
    , latitude_(latitude)
    , longitude_(longitude)
    , distances_(move(distances))
  {
  }

  std::string Name() const { return name_; }
  double Latitude() const { return latitude_; }
  double Longitude() const { return longitude_; }
  const auto& Distances() const { return distances_; }

private:
  std::string name_;
  double latitude_;
  double longitude_;
  std::unordered_map<std::string, double> distances_;
};

struct NewBusCommand : public InCommand {
public: 
  NewBusCommand(std::string name, std::vector<std::string> stops, bool is_cyclic)
    : InCommand(InCommandType::NEW_BUS)
    , name_(move(name))
    , stops_(move(stops))
    , cyclic_(is_cyclic)
  {
  }

  std::string Name() const { return name_; }
  std::vector<std::string> Stops() const { return stops_; }
  bool IsCyclic() const { return cyclic_; }

private:
  std::string name_;
  std::vector<std::string> stops_;
  bool cyclic_;
};

struct StopDescriptionCommand : public OutCommand {
public: 
  StopDescriptionCommand(std::string name)
    : OutCommand(OutCommandType::STOP_DESCRIPTION)
    , name_(move(name))
  {
  }

  StopDescriptionCommand(std::string name, size_t request_id)
    : OutCommand(OutCommandType::STOP_DESCRIPTION)
    , name_(move(name))
    , request_id_(request_id)
  {
  }

  std::string Name() const { return name_; }
  size_t RequestId() const { return request_id_; }

private:
  std::string name_;
  size_t request_id_{std::numeric_limits<size_t>::max()};
};

struct BusDescriptionCommand : public OutCommand {
public: 
  BusDescriptionCommand(std::string name)
    : OutCommand(OutCommandType::BUS_DESCRIPTION)
    , name_(move(name))
  {
  }

  BusDescriptionCommand(std::string name, size_t request_id)
    : OutCommand(OutCommandType::BUS_DESCRIPTION)
    , name_(move(name))
    , request_id_(request_id)
  {
  }

  std::string Name() const { return name_; }
  size_t RequestId() const { return request_id_; }

private:
  std::string name_;
  size_t request_id_{std::numeric_limits<size_t>::max()};
};

struct StopInfo {
  std::vector<std::string> buses;
  size_t request_id;
  std::optional<std::string> error_message;
};

struct BusInfo {
  size_t route_length;
  size_t request_id;
  double curvature;
  size_t stop_count;
  size_t unique_stop_count;
  std::optional<std::string> error_message;
};
