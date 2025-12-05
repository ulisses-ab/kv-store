#include "Application.hpp"

using namespace std;

Application::Application(int port) : reactor_(port), storage_(), controller_(storage_) {
    setup_reactor_callbacks();
}

Application::~Application() {

}

void Application::run() {
    reactor_.run();
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

void Application::connect(int fd) {
    auto session_ptr = make_unique<Session>(fd);
    setup_session_callbacks(*session_ptr);
    sessions_[fd] = move(session_ptr);
}

void Application::disconnect(int fd) {
    sessions_.erase(fd);
}

void Application::receive(int fd, const RespValue& val) {
    vector<string> raw_command = resp_to_strings(val);

    if(raw_command.empty()) {
        reactor_.send(fd, RespValue::error("ERR Protocol error: invalid command format"));
        return;
    }

    controller_.receive(*sessions_[fd], raw_command);
}

void Application::setup_session_callbacks(Session& session) {
    session.on_send([this](int fd, RespValue val){
        reactor_.send(fd, val);
    });

    session.on_disconnect([this](int fd){
        reactor_.disconnect(fd);
    });
}

vector<string> Application::resp_to_strings(const RespValue &val) {
    vector<string> result;

    if(!val.is(RespType::Array)) {
        return result;
    }

    for(const auto& item : val.as_array()) {
        if(!item.is_string()) {
            return {};
        }
        result.push_back(item.as_string());
    }

    return result;
}
