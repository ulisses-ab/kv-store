#include "Subscription.hpp"

using namespace std;

Subscription::Subscription(std::function<void()> on_unsubsribe) : on_unsubscribe_(on_unsubsribe) {

}

Subscription::Subscription(Subscription &&other) : on_unsubscribe_(other.on_unsubscribe_) {
    other.on_unsubscribe_ = nullptr;
}

Subscription::~Subscription() {
    if(on_unsubscribe_) on_unsubscribe_();
}