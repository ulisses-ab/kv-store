#pragma once

#include <list>
#include <functional>
#include <typeindex>
#include <any>

#include "Subscription.hpp"

enum EventType {

};

struct Event {
    EventType type;
    const void* data;
};

class EventBus {
public:
    using Handler = std::function<void(const void*)>;

    Subscription subscribe(EventType eventType, Handler handler);

    void dispatch(const Event& event);

    int handler_count(EventType eventType);
private:
    std::unordered_map<EventType, std::list<Handler>> handlers_;
};
