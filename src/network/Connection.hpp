#pragma once

#include "resp/RespParser.hpp"
#include <deque>
#include <functional>

class Connection {
public:
    Connection(int fd);
    ~Connection();

    int get_fd() const;

    bool handle_read();
    bool handle_write();
    void send(const RespValue& val);
    void on_receive(std::function<void(const RespValue&)> handler);
    void on_need_write(std::function<void()> handler);
    void on_write_drained(std::function<void()> handler);
private:
    void set_non_blocking();

    void parse(const char* buffer, int n);

    int fd_;
    std::deque<char> write_buffer_;
    RespParser parser_;

    std::function<void(const RespValue&)> receive_handler_;
    std::function<void()> need_write_handler_;
    std::function<void()> write_drained_handler_;

};