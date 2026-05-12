#include "core/Application.hpp"
#include <signal.h>
#include "spdlog/spdlog.h"

#include <iostream>

using namespace std;

int main() {
    //spdlog::set_level(spdlog::level::trace);

    signal(SIGPIPE, SIG_IGN);

    Application app(5555);

    try {
        app.run();
    }
    catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}