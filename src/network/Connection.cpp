#include "Connection.hpp"

#include <unistd.h>
#include <stdexcept>
#include <system_error>
#include <fcntl.h>

using namespace std;

Connection::Connection(int fd) : fd_(fd) {
    if(fd < 0) {
        throw runtime_error("Connection constructor received invalid fd: " + std::to_string(fd));
    }

    set_non_blocking();
}

void Connection::set_non_blocking() {
    int flags = fcntl(fd_, F_GETFL, 0);
    if(flags < 0) {
        throw system_error(
            errno, generic_category(),
            "fcntl(F_GETFL) failed for fd " + to_string(fd_)
        );
    }

    if(fcntl(fd_, F_SETFL, flags | O_NONBLOCK) < 0) {
        throw system_error(
            errno, generic_category(),
            "fcntl(F_SETFL) failed to set O_NONBLOCK for fd " + to_string(fd_)
        );
    }
}

Connection::~Connection() {
    close(fd_);
}

int Connection::get_fd() const {
    return fd_;
}

bool Connection::handle_read() {
    char temp[4096];

    while (true) {
        int n = read(fd_, temp, sizeof(temp));
        if(n < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  
            }
            throw system_error(errno, generic_category(), "read() failed");
        }
          
        if(n == 0) {
            return true; // connection ended
        }

        parse(temp, n);
    }

    return false; // maintain connection
}

void Connection::parse(const char* buffer, int n) {
    std::optional<RespValue> parsed;

    for (int i = 0; i < n; i++) {
        try {
            parsed = parser_.parse_char(buffer[i]);
        } 
        catch (const std::exception& e) {
            parser_.reset();
            throw;
        }

        if(parsed.has_value()) {
            if(on_receive_) on_receive_(*parsed);
        }
    }
}

void Connection::handle_write() {
    char temp[4096];

    while(!write_buffer_.empty()) {
        int to_write = min(sizeof(temp), write_buffer_.size());
        copy(write_buffer_.begin(), write_buffer_.begin() + to_write, temp);

        ssize_t n = write(fd_, temp, to_write);

        if(n < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                return;
            }
            throw system_error(errno, generic_category(), "write() failed");
        }

        if(n == 0) return;

        write_buffer_.erase(write_buffer_.begin(), write_buffer_.begin() + n);
    }

    if(on_write_drained_) on_write_drained_();
}

void Connection::send(RespValue val) {
    string data = val.encode();
    write_buffer_.insert(write_buffer_.end(), data.begin(), data.end());
    if(on_need_write_) on_need_write_();
}

void Connection::on_receive(std::function<void(RespValue)> handler) {
    on_receive_ = handler;
}

void Connection::on_need_write(std::function<void()> handler) {
    on_need_write_ = handler;
}

void Connection::on_write_drained(std::function<void()> handler) {
    on_write_drained_ = handler;
}

