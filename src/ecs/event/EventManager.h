#pragma once
#include <functional>
#include "BaseEvent.h"

// Uses observer pattern
class EventManager {
public:
    /*
    template<typename EventType>
    void emit(const EventType& event) {
        // Retrieve the list of subscribers to certain events
        auto& listeners = getListeners<EventType>();

        // Loop all the subscribers to certain events
        for (auto& listener : listeners) {
            // Invoke the function here
            listener(event);
        }
    }
    */

    using Handler = std::function<void(const BaseEvent&)>;

    void emit(const BaseEvent& event) const {
        for (const auto& listener : listeners) {
            listener(event);
        }
    }

    /*
    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> callback) {
        // Pass in the callback/callable wrapper/subscription to the list of subscriptions
        getListeners<EventType>().push_back(callback);
    }
    */

    void subscribe(const Handler& callback) {
        listeners.emplace_back(callback);
    }

private:
    /*
    // Each event type essentially has its own std:vector of listeners without you having to manage it explicitly.
    // This is done using the static local.
    // std:function<void(const EventType&)> is the callable wrapper: can hold anything that can be called like a
    // function (lambda, free function, functor, etc.)
    template<typename EventType>
    std::vector<std::function<void(const EventType&)>>& getListeners() {
        static std::vector<std::function<void(const EventType&)>> listeners;
        return listeners;
    }
    */

    std::vector<Handler> listeners;
};
