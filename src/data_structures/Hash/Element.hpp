#pragma once

template <typename Key, typename Value>
struct Element
{
    Key key;
    Value value;
    Element(const Key &k, const Value &v) : key(k), value(v) {}
};