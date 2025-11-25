#pragma once

class Listener {
public:
    Listener(int port);
    ~Listener();

    int get_fd() const;
    int get_port() const;
    int accept_client();
private:
    void create_socket();
    void set_reuseaddr();
    void set_nonblocking();
    void bind_to_port();
    void start_listening();

    void close_fd();

    int fd_;
    int port_;
};