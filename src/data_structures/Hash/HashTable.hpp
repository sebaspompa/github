#pragma once

#include <list>
#include <optional>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>

#include "src/data_structures/Hash/Element.hpp"
#include "src/data_structures/Hash/Hasher.hpp"

using std::string;
using std::vector;
using std::list;

// *** Key: string ***
template <typename Key, typename Value>
class HashTable
{
private:
    vector<list<Element<Key, Value>>> buckets;

    // Cantidad de elementos almacenados en la tabla
    size_t size = 0;

    // Cantidad total de buckets disponibles
    size_t capacity;

    // Umbrales de redimensionamiento
    static constexpr float maxThreshold = 0.75f;
    static constexpr float minThreshold = 0.35f;

    // Factores de redimensionamiento
    static constexpr float increaseFactor = 2.0f;
    static constexpr float decreaseFactor = 0.5f;

    // Capacidad minima
    static constexpr size_t min_capacity = 16;

    size_t hash(const Key &key) const
    {
        Hasher hasher;
        hasher.combine(key);
        return hasher.finalize() % capacity;
    }

    // Redimensiona la tabla por un factor
    // Redimensiona la tabla por un factor
    void resize(float factor)
    {
        size_t newCapacity = std::max(static_cast<size_t>(capacity * factor), min_capacity);

        if (newCapacity == capacity)
        {
            return;
        }

        capacity = newCapacity;
        std::vector<std::list<Element<Key, Value>>> newBuckets(capacity);

        for (auto &bucket : this->buckets)
        {
            for (auto &element : bucket)
            {
                newBuckets[hash(element.key)].push_back(std::move(element));
            }
        }

        // Actualizamos la tabla
        this->buckets = std::move(newBuckets);
    }

    bool __contains(const Key &key, size_t hashValue) const
    {
        for (const auto &element : buckets[hashValue])
        {
            if (element.key == key)
            {
                return true;
            }
        }

        return false;
    }

    void init()
    {
        buckets.resize(capacity);
    }

public:
    // Crea la tabla hash con una capacidad inicial.
    HashTable(size_t capacity) : capacity(std::max(capacity, min_capacity))
    {
        init();
    }

    // Crea la tabla hash a partir de pares `key-value`.
    HashTable(const vector<Element<Key, Value>> &elements) : capacity(std::max(static_cast<size_t>(elements.size() * increaseFactor), min_capacity))
    {
        init();
        for (const Element<Key, Value> &element : elements)
        {
            insert(element.key, element.value);
        }
    }

    // Crea la tabla hash a partir de claves y con un valor por defecto.
    HashTable(const vector<Key> &keys, const Value &defaultValue) : capacity(std::max(static_cast<size_t>(keys.size() * increaseFactor), min_capacity))
    {
        init();
        for (const Key &key : keys)
        {
            insert(key, defaultValue);
        }
    }

    bool insert(const Key &key, const Value &value)
    {
        const size_t hashValue = hash(key);

        if (__contains(key, hashValue))
        {
            return false;
        }

        buckets[hashValue].emplace_back(key, value);
        size++;

        if (float(size) / capacity >= maxThreshold)
        {
            resize(increaseFactor);
        }

        return true;
    }

    bool erase(const Key &key)
    {
        auto &bucket = buckets[hash(key)];

        for (auto it = bucket.begin(); it != bucket.end(); ++it)
        {
            if (it->key == key)
            {
                bucket.erase(it);
                size--;

                if (float(size) / capacity <= minThreshold)
                {
                    resize(decreaseFactor);
                }

                return true;
            }
        }

        return false;
    }

    Value *find(const Key &key)
    {
        for (auto &element : buckets[hash(key)])
        {
            if (element.key == key)
            {
                return &element.value;
            }
        }

        return nullptr;
    }

    const Value* find(const Key &key) const
    {
        for (const auto &element : buckets[hash(key)])
        {
            if (element.key == key)
            {
                return &element.value;
            }
        }

        return nullptr;
    }

    bool contains(const Key &key) const
    {
        return __contains(key, hash(key));
    }

    const vector<list<Element<Key, Value>>> &elements() const
    {
        return buckets;
    }

    size_t getSize() const
    {
        return size;
    }

    size_t getCapacity() const
    {
        return capacity;
    }

    void removeAll()
    {
        size = 0;
        buckets.clear();
        init();
    }

    bool update(const Key &key, const Value &value)
    {
        for (auto &element : buckets[hash(key)])
        {
            if (element.key == key)
            {
                element.value = value;

                return true;
            }
        }

        return false;
    }
};