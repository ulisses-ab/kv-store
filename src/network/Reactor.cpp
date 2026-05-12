#include "Reactor.hpp"

#include <stdexcept>
#include <iostream>

using namespace std;

Reactor::Reactor(int port) : listener_(port), epoll_(listener_.get_fd()) {
    setup_epoll_callbacks();
}

void Reactor::setup_epoll_callbacks() {
    epoll_.on_accept([this](){
        accept();
    });

    epoll_.on_disconnect([this](int fd){
        disconnect(fd);
    });

    epoll_.on_read([this](int fd){
        read(fd);
    });

    epoll_.on_write([this](int fd){
        write(fd);
    });
}

Reactor::~Reactor() {

}

void Reactor::run() {
    while(true) {
        epoll_.poll();
    }
}

void Reactor::accept() {
    int fd;

    try {
        fd = listener_.accept_client();
    }
    catch(const exception& e) {
        cerr << "Listener accept() failed: " << e.what() << "\n";
        return;
    }

    try {
        auto conn_ptr = make_unique<Connection>(fd);
        setup_connection_callbacks(*conn_ptr);
        connections_[fd] = move(conn_ptr);
    }
    catch(const exception& e) {
        cerr << "Connection construction failed: " << e.what() << "\n";
        return;
    }

    try {
        epoll_.add_client(fd);
    }
    catch(const exception& e) {
        disconnect(fd);
        cerr << "Failed to add client to epoll: " << e.what() << "\n";
        return;
    }

    if(connect_handler_) connect_handler_(fd);
}   

void Reactor::setup_connection_callbacks(Connection& conn) {
    int fd = conn.get_fd();

    conn.on_receive([this, fd](const RespValue& val){
        if(receive_handler_) receive_handler_(fd, val);
    });

    conn.on_need_write([this, fd](){
        try {
            epoll_.want_write(fd);
        } catch (const std::exception& e) {
            cerr << "Error in epoll.want_write() for fd " << fd
            << ": " << e.what() << "\n";
            disconnect(fd);
        }
    });

    conn.on_write_drained([this, fd](){
        try {
            epoll_.stop_write(fd);
        } catch (const std::exception& e) {
            cerr << "Error in epoll.stop_write() for fd " << fd
            << ": " << e.what() << "\n";
            disconnect(fd);
        }
    });
}


void Reactor::read(int fd) {
    auto it = connections_.find(fd);
    if(it == connections_.end()) {
        cerr << "Read event on non-existent connection: fd " << fd << "\n";
        return;
    }

    try {
        if(it->second->handle_read()) {
            disconnect(fd);
        }
    }
    catch(const exception& e) {
        cerr << "Failed to handle read on fd " << fd << ": " << e.what() << "\n";
    }
}

void Reactor::write(int fd) {
    auto it = connections_.find(fd);
    if(it == connections_.end()) {
        cerr << "Write event on non-existent connection: fd " << fd << "\n";
        return;
    }

    try {
        if(it->second->handle_write()) {
            disconnect(fd);
        }
    }
    catch(const exception& e) {
        cerr << "Failed to handle write on fd " << fd << "\n";
    }
}

void Reactor::disconnect(int fd) {
    connections_.erase(fd);

    try {
        epoll_.remove_client(fd);
    } 
    catch (const std::exception& e) {
        cerr << "Failed to remove fd " << fd
        << " from epoll: " << e.what() << "\n";
    }

    if(disconnect_handler_) disconnect_handler_(fd);
}

void Reactor::send(int fd, RespValue val) {
    auto it = connections_.find(fd);

    if(it == connections_.end()) {
        cerr << "Tried sending value to non-existent fd " << fd << "\n";
    }

    it->second->send(val);
}

void Reactor::on_connect(std::function<void(int)> handler) {
    connect_handler_ = handler;
}

void Reactor::on_disconnect(std::function<void(int)> handler) {
    disconnect_handler_ = handler;
}

void Reactor::on_receive(std::function<void(int, const RespValue&)> handler) {
    receive_handler_ = handler;
}

