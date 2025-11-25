#include "network/Reactor.hpp"

#include <iostream>

using namespace std;

int main() {
    Reactor reactor(5555);

    reactor.on_connect([&](int fd){
        cout << "Client connected: fd " << fd << endl;

        reactor.send(fd, RespValue::simple_string("OK"));
    });

    reactor.on_disconnect([](int fd){
        cout << "Client disconnected: fd " << fd << endl;
    });

    reactor.on_receive([](int fd, RespValue val){
        cout << "Value received on fd " << fd << ":\n" 
        << val.display() << endl;
    });

    reactor.run();

    return 0;
}