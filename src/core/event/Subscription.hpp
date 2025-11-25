#pragma once

#include <functional>

class Subscription {
public:
    explicit Subscription(std::function<void()> on_unsubsribe);
    Subscription(Subscription&& other);
    ~Subscription();
private:
    std::function<void()> on_unsubscribe_;
};