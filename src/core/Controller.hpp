#pragma once

#include "../core/Session.hpp"
#include "../storage/Storage.hpp"
#include "event/EventBus.hpp"
#include "command/CommandDispatcher.hpp"

class Controller {
public:
    Controller(Storage& storage);

    void receive(Session& session, const std::vector<std::string>& raw_command);
    void disconnect(const Session& session);
private:
    Storage& storage_;
    EventBus event_bus_;
    CommandDispatcher command_dispatcher_;
};