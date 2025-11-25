#include "Application.hpp"

Application::Application(int port) : reactor_(port), storage_(), controller_(storage_) {
    setup_reactor_callbacks();
}

void Application::setup_reactor_callbacks() {
    reactor_.on_connect([this](int fd){
        connect(fd);
    });

    reactor_.on_disconnect([this](int fd){
        disconnect(fd);
    });

    reactor_.on_receive([this](int fd, RespValue val){
        receive(fd, val);
    });
}

void Application::setup_controller_callbacks() {

}

