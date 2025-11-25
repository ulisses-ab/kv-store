#include "EventBus.hpp"

template<typename Event>
Subscription EventBus::subscribe(Handler<Event> handler) {
    std::type_index type = typeid(Event);

    auto& lst = handlers_[type];
    auto it = lst.insert(lst.end(), move(handler));

    return Subscription([this, it, type](){
        handlers_[type].erase(it);
    });
}

template<typename Event>
void EventBus::dispatch(const Event& event) {
    auto it = handlers_.find(typeid(Event));
    if (it == handlers_.end()) return;

    for(auto& handler : it->second) {
        any_cast<Handler<Event>&>(handler)(event);
    }
}

template<typename Event>
int EventBus::handler_count() {
    auto it = handlers_.find(typeid(Event));
    if (it == handlers_.end()) return 0;

    return it->second.size();
}