#include "Session.hpp"

Session::Session(int fd) : fd_(fd) {

}

void Session::send(const RespValue& val) {
    if(send_handler_) send_handler_(fd_, val);
}

void Session::disconnect() {
    if(disconnect_handler_) disconnect_handler_(fd_);
}

void Session::on_send(std::function<void(int, const RespValue&)> handler) {
    send_handler_ = handler;
}

void Session::on_disconnect(std::function<void(int)> handler) {
    disconnect_handler_ = handler;
}

int Session::get_fd() { 
    return fd_;
}
