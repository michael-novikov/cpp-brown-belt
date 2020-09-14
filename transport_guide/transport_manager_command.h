#pragma once

#include <string>

enum class CommandTypes {
  NEW_STOP,
  NEW_BUS,

  BUS_DESCRIPTION,

  ERROR_COMMAND,
  NUM_COMMANDS,
};

struct TransportManagerCommand {
public:
  const static CommandTypes type{CommandTypes::ERROR_COMMAND};
};

struct NewStopCommand : public TransportManagerCommand {
public: 
  NewStopCommand(std::string name,double latitude, double longitude);
  const static CommandTypes type{CommandTypes::NEW_STOP};

  std::string name;
  double latitude;
  double longitude;
};
