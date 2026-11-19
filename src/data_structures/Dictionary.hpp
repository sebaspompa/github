#pragma once

#include <optional>
#include <stdexcept>

using std::optional;
using std::nullopt;

#include "src/data_structures/Hash/HashTable.hpp"

// *** Key: string ***
template <typename Key, typename Value>
class Dictionary
{
private:
    static constexpr size_t initialSize = 121;

    HashTable<Key, Value> hashTable;

    // Iterador para acceder, por referencia, a todos los pares `key-value` del diccionario.
    class const_iterator
    {
    public:
        using BucketIt = typename vector<list<Element<Key, Value>>>::const_iterator;
        using ListIt = typename list<Element<Key, Value>>::const_iterator;

        const_iterator(BucketIt bIt, BucketIt bEnd)
            : bucketIt(bIt), bucketEnd(bEnd)
        {
            advanceToNextNonEmpty();
        }

        const Element<Key, Value> &operator*() const
        {
            return *listIt;
        }

        const_iterator &operator++()
        {
            ++listIt;
            if (listIt == bucketIt->end())
            {
                ++bucketIt;
                advanceToNextNonEmpty();
            }
            return *this;
        }

        bool operator!=(const const_iterator &other) const
        {
            return bucketIt != other.bucketIt || (bucketIt != bucketEnd && listIt != other.listIt);
        }

        const Element<Key, Value> *operator->() const
        {
            return &(*listIt);
        }

    private:
        BucketIt bucketIt, bucketEnd;
        ListIt listIt;

        void advanceToNextNonEmpty()
        {
            while (bucketIt != bucketEnd && bucketIt->empty())
            {
                ++bucketIt;
            }
            if (bucketIt != bucketEnd)
            {
                listIt = bucketIt->begin();
            }
        }
    };

public:
    // Crea un diccionario vacio. 
    Dictionary() : hashTable(initialSize) {}

    // Crea un diccionario a partir de pares `key-value`.
    Dictionary(const vector<Element<Key, Value>> &values) : hashTable(values) {}

    // Crea un diccionario a partir de claves y con un valor por defecto.
    Dictionary(const vector<Key> &keys, const Value &defaultValue) : hashTable(keys, defaultValue) {}

    void add(const Key &key, const Value &value)
    {
        if (!hashTable.insert(key, value))
        {
            throw std::invalid_argument("La clave ya existe en el diccionario");
        }
    }

    void set(const Key &key, const Value &value)
    {
        if (!hashTable.update(key, value))
        {
            if(!hashTable.insert(key, value))
            {
                throw std::runtime_error("Error al insertar o actualizar el elemento");
            }
        }
    }

    void remove(const Key &key)
    {
        if(!hashTable.erase(key))
        {
            throw std::runtime_error("Error al eliminar el elemento");
        }
    }

    bool contains(const Key &key) const
    {
        return hashTable.contains(key);
    }

    optional<Value> get(const Key &key) const
    {
        const Value *ptr = hashTable.find(key);

        if (ptr == nullptr)
        {
            return nullopt;
        }

        return *ptr;
    }

    optional<Element<Key, Value>> pop()
    {
        auto it = begin();
        if (it == end())
        {
            return nullopt;
        }

        Element<Key, Value> extracted = *it;
        hashTable.erase(extracted.key);

        return extracted;
    }

    size_t count() const
    {
        return hashTable.getSize();
    }

    bool isEmpty() const
    {
        return hashTable.getSize() == 0;
    }

    // Retorna una copia de todos los pares `key-value` del diccionario.
    vector<Element<Key, Value>> items() const
    {
        vector<Element<Key, Value>> _items;
        _items.reserve(count());

        for (const auto &element : *this)
        {
            _items.push_back(element);
        }
        return _items;
    }

    void clear()
    {
        hashTable.removeAll();
    }

    const_iterator begin() const
    {
        const auto &buckets = hashTable.elements();
        return const_iterator(buckets.begin(), buckets.end());
    }

    const_iterator end() const
    {
        const auto &buckets = hashTable.elements();
        return const_iterator(buckets.end(), buckets.end());
    }
};
