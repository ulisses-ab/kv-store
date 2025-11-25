#include "Epoll.hpp"

#include <system_error>
#include <unistd.h>

using namespace std;

Epoll::Epoll(int listener_fd) : epoll_fd_(-1), listener_fd_(listener_fd) {
    create_epoll_fd();
    register_listener();
}

Epoll::~Epoll() {
    close_fd();
}

void Epoll::create_epoll_fd() {
  epoll_fd_ = epoll_create1(0);
  if (epoll_fd_ < 0) {
    throw system_error(errno, generic_category(), "epoll_create1() failed");
  }
}

void Epoll::register_listener() {
    struct epoll_event event {};
    event.events = EPOLLIN;
    event.data.fd = listener_fd_;

    if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listener_fd_, &event) < 0) {
        close_fd();
        throw system_error(errno, generic_category(), 
            "epoll_ctl(ADD) failed for fd " + to_string(listener_fd_)
        );
    }
}

void Epoll::poll() {
    int n = epoll_wait(epoll_fd_, events_, MAX_EVENTS, 0);
    if(n < 0) {
        throw system_error(errno, generic_category(), 
            "epoll_wait() failed"
        );
    }

    handle_events(n);
}

void Epoll::handle_events(int n) {
    for (int i = 0; i < n; i++) {
        process_event(events_[i]);
    }
}

void Epoll::process_event(const epoll_event &e) {
    int fd = e.data.fd;
    uint32_t ev = e.events;

    if(ev & (EPOLLERR | EPOLLHUP)) return handle_disconnect(fd);

    if(fd == listener_fd_ && (ev & EPOLLIN)) {
        if(on_accept_) on_accept_();
        return;
    }

    if(ev & EPOLLIN) { 
        if(on_read_) on_read_(fd); 
        return; 
    }

    if(ev & EPOLLOUT) { 
        if(on_write_) on_write_(fd); 
        return; 
    }

    throw runtime_error("Unexpected epoll event flags: " + to_string(ev));
}

void Epoll::handle_disconnect(int fd) {
    if(fd == listener_fd_) throw runtime_error("Fatal: listener closed");
    remove_client(fd);
    if(on_disconnect_) on_disconnect_(fd);
}


void Epoll::add_client(int client_fd) {
    struct epoll_event event {};
    event.data.fd = client_fd;
    event.events = EPOLLIN | EPOLLET;

    if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &event) < 0) {
        throw system_error(errno, generic_category(), 
            "epoll_ctl(ADD) failed for fd " + to_string(client_fd)
        );
    }
}

void Epoll::remove_client(int client_fd) {
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, nullptr) < 0) {
        if (errno != EBADF) {
            throw system_error(errno, generic_category(),
                "epoll_ctl(DEL) failed for fd " + to_string(client_fd)
            );
        }
    }
}

void Epoll::want_write(int client_fd) {
    struct epoll_event event {};
    event.data.fd = client_fd;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, client_fd, &event) < 0) {
        throw system_error(errno, generic_category(),
            "epoll_ctl(MOD) failed in want_write() for fd " + to_string(client_fd)
        );
    }
}

void Epoll::stop_write(int client_fd) {
    struct epoll_event event {};
    event.data.fd = client_fd;
    event.events = EPOLLIN | EPOLLET;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, client_fd, &event) < 0) {
        throw system_error(errno, generic_category(),
            "epoll_ctl(MOD) failed in stop_write() for fd " + to_string(client_fd)
        );
    }
}

void Epoll::on_accept(function<void()> handler) {
    on_accept_ = handler;
}

void Epoll::on_read(function<void(int)> handler) {
    on_read_ = handler;
}

void Epoll::on_write(function<void(int)> handler) {
    on_write_ = handler;
}

void Epoll::on_disconnect(function<void(int)> handler) {
    on_disconnect_ = handler;
}

void Epoll::close_fd() {
    if(epoll_fd_ >= 0) close(epoll_fd_);
    epoll_fd_ = -1;
}
