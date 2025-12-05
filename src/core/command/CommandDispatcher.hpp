#pragma once

#include "Command.hpp"
#include <memory>
#include <unordered_map>

class CommandDispatcher {
public:
    CommandDispatcher();

    std::vector<Event> dispatch(Session& session, Storage& storage, const std::vector<std::string>& raw);

    void register_command(std::string name, std::unique_ptr<Command> command);
private:
    std::unordered_map<std::string, std::unique_ptr<Command>> registered_commands_;
};