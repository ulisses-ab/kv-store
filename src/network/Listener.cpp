#include "Listener.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdexcept>
#include <system_error>
#include <fcntl.h>

using namespace std;

Listener::Listener(int port) : fd_(-1), port_(port) {
    if(port < 1024 || port > 65535) {
        throw invalid_argument("Port must be between 1024 and 65535");
    }

    create_socket();
    set_reuseaddr();
    set_nonblocking();
    bind_to_port();
    start_listening();
}

void Listener::create_socket() {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(fd_ < 0) {
        throw system_error(errno, generic_category(),
            "socket() failed while creating listener socket"
        );
    }
}

void Listener::set_reuseaddr() {
    int reuse = 1;
    if(setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        close_fd();
        throw system_error(errno, generic_category(),
            "setsockopt(SO_REUSEADDR) failed"
        );

    }
}

void Listener::set_nonblocking() {
    int flags = fcntl(fd_, F_GETFL, 0);
    if (flags < 0) {
        throw system_error(errno, generic_category(), "fcntl(F_GETFL) failed");
    }

    if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) < 0) {
        throw system_error(errno, generic_category(), "fcntl(F_SETFL, O_NONBLOCK) failed");
    }
}

void Listener::bind_to_port() {
    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if(bind(fd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close_fd();
        throw system_error(errno, generic_category(),
            "bind() failed for port " + to_string(port_)
        );

    }
}

void Listener::start_listening() {
    if(listen(fd_, 5) < 0) {
        close_fd();
        throw system_error(errno, generic_category(),
            "listen() failed on port " + to_string(port_)
        );
    }
}

int Listener::accept_client() {
    int client_fd = accept(fd_, nullptr, nullptr);

    if(client_fd < 0) {
        throw system_error(errno, generic_category(),
            "accept() failed while waiting for incoming connection"
        );
    }

    return client_fd;
}

Listener::~Listener() {
    close_fd();
}

void Listener::close_fd() {
    if(fd_ >= 0) close(fd_);
    fd_ = -1;
}

int Listener::get_fd() const {
    return fd_;
}

int Listener::get_port() const {
    return port_;
}

