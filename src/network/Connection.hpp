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
    void handle_write();
    void send(RespValue val);
    void on_receive(std::function<void(RespValue)> handler);
    void on_need_write(std::function<void()> handler);
    void on_write_drained(std::function<void()> handler);
private:
    void set_non_blocking();

    void parse(const char* buffer, int n);

    int fd_;
    std::deque<char> write_buffer_;
    RespParser parser_;

    std::function<void(RespValue)> on_receive_;
    std::function<void()> on_need_write_;
    std::function<void()> on_write_drained_;

};