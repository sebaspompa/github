#pragma once
#include <iostream>
#include <functional>
#include <optional>

using namespace std;
using UInt = unsigned int;

template <typename T>
class Queue
{
public:
    struct Node
    {
        T data;
        Node *next;

        Node(T data, Node *next = nullptr) : data(data), next(next) {}
    };

public:
    Node *front;
    Node *rear;
    UInt _length;

public:
    Queue() : front(nullptr), rear(nullptr), _length(0) {}

    ~Queue()
    {
        Node *tmp = front;

        while (tmp != nullptr)
        {
            front = front->next;
            delete tmp;
            tmp = front;
        }

        rear = nullptr;
        _length = 0;
    }

    // Se elimina la copia por construcción.
    Queue(const Queue &) = delete;

    // Se elimina la copia por asignación.
    Queue &operator=(const Queue &) = delete;

    // Permite construir una cola en base a las propiedades de otra.
    Queue(Queue &&other) noexcept : front(other.front), rear(other.rear), _length(other._length)
    {
        other.front = nullptr;
        other.rear = nullptr;
        other._length = 0;
    }

    // Permite transferir la información de una cola a la actual.
    Queue &operator=(Queue &&other) noexcept
    {
        if (this != &other)
        {
            while (front != nullptr)
            {
                Node *tmp = front;
                front = front->next;
                delete tmp;
            }

            rear = nullptr;
            _length = 0;

            front = other.front;
            rear = other.rear;
            _length = other._length;

            other.front = nullptr;
            other.rear = nullptr;
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

    void enqueue(T data)
    {
        Node *newNode = new Node(data);

        if (rear == nullptr)
        {
            front = newNode;
            rear = newNode;
        }
        else
        {
            rear->next = newNode;
            rear = newNode;
        }

        _length++;
    }

    optional<T> dequeue()
    {
        if (front == nullptr)
        {
            return nullopt;
        }

        Node *tmp = front;
        T value = tmp->data;

        front = front->next;

        if (front == nullptr)
        {
            rear = nullptr;
        }

        delete tmp;
        _length--;

        return value;
    }

    optional<T> frontValue()
    {
        if (front == nullptr)
        {
            return nullopt;
        }

        return front->data;
    }

    optional<T> rearValue()
    {
        if (rear == nullptr)
        {
            return nullopt;
        }

        return rear->data;
    }

    // T : operator ==
    bool contains(const T &data) const
    {
        Node *tmp = front;

        while (tmp != nullptr)
        {
            if (tmp->data == data)
            {
                return true;
            }

            tmp = tmp->next;
        }

        return false;
    }

    // Retorna una cola con todos los elementos que satisfacen el predicado dado.
    Queue<T> filter(function<bool(T)> predicate)
    {
        Queue<T> items;

        Node *tmp = front;

        while (tmp != nullptr)
        {
            if (predicate(tmp->data))
            {
                items.enqueue(tmp->data);
            }

            tmp = tmp->next;
        }

        return items;
    }

    void forEach(function<void(T)> closure) const
    {
        Node *tmp = front;

        while (tmp != nullptr)
        {
            closure(tmp->data);
            tmp = tmp->next;
        }
    }
};