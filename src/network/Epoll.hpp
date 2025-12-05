#pragma once

#include <functional>
#include <sys/epoll.h>
#include <set>

class Epoll {
public:
    Epoll(int listener_fd);
    ~Epoll();

    void poll();
    void add_client(int fd);
    void remove_client(int fd);
    void want_write(int fd);
    void stop_write(int fd);

    void on_accept(std::function<void()>);
    void on_read(std::function<void(int)>);
    void on_write(std::function<void(int)>);
    void on_disconnect(std::function<void(int)>);
private:
    void create_epoll_fd();
    void register_listener();
    void handle_events(int n);
    void process_event(const epoll_event& e);
    void handle_disconnect(int fd);

    void close_fd();

    int epoll_fd_;
    int listener_fd_;

    static const size_t MAX_EVENTS = 1024;
    struct epoll_event events_[MAX_EVENTS];

    std::function<void()> accept_handler_;
    std::function<void(int)> read_handler_;
    std::function<void(int)> write_handler_;
    std::function<void(int)> disconnect_handler_;
};