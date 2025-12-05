#include "Subscription.hpp"

using namespace std;

Subscription::Subscription(std::function<void()> on_unsubsribe) : unsubscribe_handler_(on_unsubsribe) {

}

Subscription::Subscription(Subscription &&other) : unsubscribe_handler_(other.unsubscribe_handler_) {
    other.unsubscribe_handler_ = nullptr;
}

Subscription::~Subscription() {
    if(unsubscribe_handler_) unsubscribe_handler_();
}