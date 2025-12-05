#pragma once

#include <functional>
#include "../network/resp/RespValue.hpp"

class Session {
public:
    Session(int fd);

    void send(const RespValue& val);
    void disconnect();

    void on_send(std::function<void(int, const RespValue&)> handler);
    void on_disconnect(std::function<void(int)> handler);

    int get_fd();
private:
    int fd_;

    std::function<void(int, const RespValue&)> send_handler_;
    std::function<void(int)> disconnect_handler_;
};