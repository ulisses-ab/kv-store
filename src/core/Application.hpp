#pragma once

#include "../network/Reactor.hpp"
#include "../storage/Storage.hpp"
#include "Session.hpp"
#include "Controller.hpp"
#include <memory>

class Application {
public:
    Application(int port);
    ~Application();

    void run();
private:
    void connect(int fd);
    void disconnect(int fd);
    void receive(int fd, const RespValue& val);

    void setup_reactor_callbacks();
    void setup_session_callbacks(Session& session);

    std::vector<std::string> resp_to_strings(const RespValue& val);

    Reactor reactor_;
    Storage storage_;
    Controller controller_;
    std::unordered_map<int, std::unique_ptr<Session>> sessions_;
};