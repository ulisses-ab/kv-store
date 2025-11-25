#pragma once

#include "event/EventBus.hpp"
#include "../network/Reactor.hpp"
#include "../storage/Storage.hpp"
#include "Session.hpp"
#include "Controller.hpp"

class Application {
public:
    Application(int port);
    ~Application();

    void run();
private:
    void connect(int fd);
    void disconnect(int fd);
    void receive(int fd, RespValue val);
    void send(int fd, RespValue val);

    void setup_reactor_callbacks();
    void setup_controller_callbacks();

    Reactor reactor_;
    Storage storage_;
    Controller controller_;
    std::unordered_map<int, Session> sessions_;
};