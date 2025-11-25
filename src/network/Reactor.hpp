#pragma once

#include <functional>
#include <unordered_map>
#include "Connection.hpp"
#include "Listener.hpp"
#include "Epoll.hpp"
#include <memory>

class Reactor {
public:
    Reactor(int port);
    ~Reactor();

    void on_connect(std::function<void(int)> handler);
    void on_disconnect(std::function<void(int)> handler);
    void on_receive(std::function<void(int, RespValue)> handler);

    void disconnect(int fd);
    void send(int fd, RespValue val);

    void run();
private:
    void accept();
    void read(int fd);
    void write(int fd);

    void setup_epoll_callbacks();
    void setup_connection_callbacks(Connection& conn);

    Listener listener_;
    std::unordered_map<int, std::unique_ptr<Connection>> connections_;
    Epoll epoll_;

    std::function<void(int)> on_connect_;
    std::function<void(int)> on_disconnect_;
    std::function<void(int, RespValue)> on_receive_;
};