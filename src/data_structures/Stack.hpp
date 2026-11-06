#pragma once
#include <optional>
#include <stdexcept>

using std::optional;
using std::nullopt;
using std::runtime_error;
using UInt = unsigned int;

template <typename T>
class Stack
{
private:
    struct Node
    {
        T data;
        Node *next;

        Node(T data, Node *next = nullptr)
            : data(data), next(next) {}
    };

    Node *topNode;
    UInt _length;

public:
    Stack() : topNode(nullptr), _length(0) {}

    ~Stack()
    {
        while (topNode != nullptr)
        {
            Node *tmp = topNode;
            topNode = topNode->next;
            delete tmp;
        }
    }

    // Se elimina la copia por construcción
    Stack(const Stack &) = delete;

    // Se elimina la copia por asignación
    Stack &operator=(const Stack &) = delete;

    // Permite construir una pila en base a las propiedades de otra.
    Stack(Stack &&other) noexcept
        : topNode(other.topNode), _length(other._length)
    {
        other.topNode = nullptr;
        other._length = 0;
    }

    // Permite transferir la informacion de una pila a la actual.
    Stack &operator=(Stack &&other) noexcept
    {
        if (this != &other)
        {
            while (topNode != nullptr)
            {
                Node *tmp = topNode;
                topNode = topNode->next;
                delete tmp;
            }

            topNode = other.topNode;
            _length = other._length;

            other.topNode = nullptr;
            other._length = 0;
        }

        return *this;
    }

    UInt length() const
    {
        return _length;
    }

    bool isEmpty() const
    {
        return _length == 0;
    }

    void push(T data)
    {
        Node *newNode = new Node(data, topNode);
        topNode = newNode;
        _length++;
    }

    optional<T> pop()
    {
        if (topNode == nullptr)
        {
            return {};
        }

        Node *tmp = topNode;
        T value = tmp->data;

        topNode = topNode->next;

        delete tmp;
        _length--;

        return value;
    }

    optional<T> top() const
    {
        if (topNode == nullptr)
        {
            return {};
        }

        return topNode->data;
    }

    T& peek() const
    {
        if (topNode == nullptr)
        {
            throw runtime_error("Stack vacio.");
        }

        return topNode->data;
    }

    void clear()
    {
        while (!isEmpty())
        {
            pop();
        }
    }
};