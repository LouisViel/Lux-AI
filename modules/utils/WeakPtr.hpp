#pragma once
#include <unordered_set>
#include <memory>

// Hash générique pour std::weak_ptr<T>
template <typename T>
struct WeakPtrHash {
    std::size_t operator()(const std::weak_ptr<T>& w) const noexcept {
        if (std::shared_ptr<T> s = w.lock()) {
            return std::hash<T*>()(s.get());
        } return 0; // hash neutre si expiré
    }
};

// Comparateur générique pour std::weak_ptr<T>
template <typename T>
struct WeakPtrEqual {
    bool operator()(const std::weak_ptr<T>& a, const std::weak_ptr<T>& b) const noexcept {
        return !a.owner_before(b) && !b.owner_before(a);
    }
};

template <typename T>
using WeakPtrUnorderedSet = std::unordered_set<
    std::weak_ptr<T>,
    WeakPtrHash<T>,
    WeakPtrEqual<T>
>;