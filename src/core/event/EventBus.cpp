#include "EventBus.hpp"

Subscription EventBus::subscribe(EventType eventType, Handler handler) {
    auto& lst = handlers_[eventType];
    auto it = lst.insert(lst.end(), move(handler));

    return Subscription([it, &lst](){
        lst.erase(it);
    });
}

void EventBus::dispatch(const Event& event) {
    auto it = handlers_.find(event.type);
    if (it == handlers_.end()) return;

    for(auto& handler : it->second) {
        handler(event.data);
    }
}

int EventBus::handler_count(EventType eventType) {
    auto it = handlers_.find(eventType);
    if (it == handlers_.end()) return 0;

    return it->second.size();
}