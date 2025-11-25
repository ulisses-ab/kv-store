#pragma once

#include "Session.hpp"
#include "../storage/Storage.hpp"
#include "../network/resp/RespValue.hpp"
#include "event/EventBus.hpp"

class Controller {
public:
    Controller(Storage& storage);

    void receive(const Session& session, RespValue val);

    void on_send(std::function<void(int, RespValue)> handler);
    void on_disconnect(std::function<void(int)> handler);
private:
    Storage& storage_;
    EventBus event_bus_;
};