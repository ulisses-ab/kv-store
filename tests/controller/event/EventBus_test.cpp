#include "catch2.hpp"
#include "controller/event/EventBus.hpp"

using namespace std;

TEST_CASE("EventBusTest", "[EventBus]") {
    EventBus bus;

    struct TestEvent {
        TestEvent(string s) : message(s) {}
        string message;
    };

    {
        string res;
        Subscription sub = bus.subscribe<TestEvent>([&](const TestEvent& e){
            res = e.message;
        });

        bus.dispatch(TestEvent("aura"));

        REQUIRE(res == "aura");
    };

    REQUIRE(bus.handler_count<TestEvent>() == 0);
}