#pragma once

#include <list>
#include <functional>
#include <typeindex>
#include <any>

#include "Subscription.hpp"

class EventBus {
public:
    template<typename Event>
    using Handler = std::function<void(const Event&)>;

    template<typename Event>
    Subscription subscribe(Handler<Event> handler);

    template<typename Event>
    void dispatch(const Event& event);

    template<typename Event>
    int handler_count();
private:
    std::unordered_map<std::type_index, std::list<std::any>> handlers_;
};

#include "EventBus.tpp"