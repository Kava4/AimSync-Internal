#pragma once

#include <cstdint>
#include <optional>

namespace cs2
{

// Schema CUtlMap layout used by CEconItemSchema maps.
template <typename K, typename V>
struct EconUtlMap {
    struct Node {
        int left;
        int right;
        int parent;
        int tag;
        K key;
        V value;
    };

    int size{};
    int unknown{};
    Node* data{};
    int root{};

    [[nodiscard]] Node* begin() const noexcept { return data; }
    [[nodiscard]] Node* end() const noexcept { return data ? data + size : nullptr; }

    [[nodiscard]] std::optional<V> findByKey(K key) const noexcept
    {
        int current = root;
        while (current != -1 && data) {
            const Node& element = data[current];
            if (element.key < key)
                current = element.right;
            else if (element.key > key)
                current = element.left;
            else
                return element.value;
        }
        return std::nullopt;
    }

    template <typename Predicate>
    [[nodiscard]] V findValue(Predicate predicate) const noexcept
    {
        if (!data)
            return V{};

        for (int i = 0; i < size; ++i) {
            if (!predicate(data[i]))
                continue;
            return data[i].value;
        }
        return V{};
    }
};

// Shared-object type-cache object vector (cs2-sdk layout).
template <typename T>
struct EconObjectVector {
    int size{};
    char pad0[4]{};
    T* data{};
    char pad1[8]{};

    [[nodiscard]] T* begin() const noexcept { return data; }
    [[nodiscard]] T* end() const noexcept { return data ? data + size : nullptr; }
};

}
