#include "Connection.hpp"

#include <unistd.h>
#include <stdexcept>
#include <system_error>
#include <fcntl.h>
#include "spdlog/spdlog.h"

using namespace std;

Connection::Connection(int fd) : fd_(fd) {
    if(fd < 0) {
        throw runtime_error("Connection constructor received invalid fd: " + std::to_string(fd));
    }

    spdlog::debug("Connection constructed on fd {}", fd_);

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

    spdlog::debug("fd {} set to non-blocking mode", fd_);
}

Connection::~Connection() {
    spdlog::debug("Connection destructed, closing fd {}", fd_);
    close(fd_);
}

int Connection::get_fd() const {
    return fd_;
}

bool Connection::handle_read() {
    spdlog::trace("handle_read() called on fd {}", fd_);
    char temp[4096];

    while (true) {
        int n = read(fd_, temp, sizeof(temp));
        if(n < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  
            }
            spdlog::error("read() failed on fd {}: {}", fd_, strerror(errno));
            throw system_error(errno, generic_category(), "read() failed");
        }
          
        if(n == 0) {
            spdlog::trace("Connection closed by peer on fd {}", fd_);
            return true; // connection ended
        }

        spdlog::trace("Read {} bytes from fd {}", n, fd_);
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
            spdlog::warn("Parser error on fd {}: {}", fd_, e.what());
            throw;
        }

        if(parsed.has_value()) {
            spdlog::debug("Parsed RespValue on fd {}", fd_);
            if(receive_handler_) receive_handler_(*parsed);
        }
    }
}

bool Connection::handle_write() {
    spdlog::trace("handle_write() called on fd {}", fd_);
    char temp[4096];

    while(!write_buffer_.empty()) {
        int to_write = min(sizeof(temp), write_buffer_.size());
        copy(write_buffer_.begin(), write_buffer_.begin() + to_write, temp);

        ssize_t n = write(fd_, temp, to_write);

        if(n < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                spdlog::trace("fd {} write would block, returning", fd_);
                return false; // maintain connection
            }
            if(errno == EPIPE) {
                spdlog::info("Broken pipe (EPIPE) on fd {}", fd_);
                write_buffer_.clear();
                if(write_drained_handler_) write_drained_handler_();
                return true; // connection closed
            }
            spdlog::error("write() failed on fd {}: {}", fd_, strerror(errno));
            throw system_error(errno, generic_category(), "write() failed");
        }

        if(n == 0) break;

        write_buffer_.erase(write_buffer_.begin(), write_buffer_.begin() + n);
        spdlog::trace("Wrote {} bytes to fd {}, remaining {}", n, fd_, write_buffer_.size());
    }

    if(write_drained_handler_) {
        spdlog::debug("Write buffer drained on fd {}", fd_);
        write_drained_handler_();
    }

    return false; // maintain connection
}

void Connection::send(const RespValue& val) {
    string data = val.encode();
    write_buffer_.insert(write_buffer_.end(), data.begin(), data.end());
    spdlog::debug("Queued {} bytes for fd {}, buffer size now {}", data.size(), fd_, write_buffer_.size());
    if(need_write_handler_) need_write_handler_();
}

void Connection::on_receive(std::function<void(const RespValue&)> handler) {
    receive_handler_ = handler;
}

void Connection::on_need_write(std::function<void()> handler) {
    need_write_handler_ = handler;
}

void Connection::on_write_drained(std::function<void()> handler) {
    write_drained_handler_ = handler;
}
