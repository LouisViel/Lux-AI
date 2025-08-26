#pragma once
#include <functional>
#include <vector>
#include <algorithm>
#include <mutex>

class Event {
private:
    using Handler = std::function<void()>;
    std::vector<Handler> handlers;
    std::mutex mutex;

public:
    void subscribe(const Handler& handler)
    {
        std::lock_guard<std::mutex> lock(mutex);
        handlers.push_back(handler);
    }

    void unsubscribe(const Handler& handler)
    {
        std::lock_guard<std::mutex> lock(mutex);
        handlers.erase(
            std::remove_if(handlers.begin(), handlers.end(),
                [&](const Handler& h) {
                    // Compare les pointeurs internes de std::function
                    return h.target<void()>() == handler.target<void()>();
                }),
            handlers.end());
    }

    void invoke()
    {
        std::vector<Handler> snapshot;
        {
            // copie pour éviter deadlock si un handler s'abonne/désabonne
            std::lock_guard<std::mutex> lock(mutex);
            snapshot = handlers;
        }

        for (auto& handler : snapshot) {
            if (handler) handler();
        }
            
    }

    void operator+=(const Handler& handler)
    {
        subscribe(handler);
    }

    void operator-=(const Handler& handler)
    {
        unsubscribe(handler);
    }

    void operator()()
    {
        invoke();
    }
};