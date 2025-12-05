#include "Controller.hpp"

using namespace std;

Controller::Controller(Storage& storage) : storage_(storage) {

}   

void Controller::receive(Session& session, const std::vector<std::string>& raw_command) {
    command_dispatcher_.dispatch(session, storage_, raw_command);
}

