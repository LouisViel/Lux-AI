#pragma once
#include <memory>

// make_unique universel : supporte tous les C++ et forwarding
#if __cplusplus <= 201103L
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#else
using std::make_unique;
#endif

#define make_unique_friend \
    template<typename T, typename... Args> \
    friend std::unique_ptr<T> make_unique(Args&&... args);
