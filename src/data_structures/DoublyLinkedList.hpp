#pragma once
#include <functional>
#include <optional>

using std::optional;
using std::function;
using UInt = unsigned int;

template <typename T>
class DoublyLinkedList
{
public:
    struct Node
    {
        T data;
        Node* next;
        Node* prev;

        Node(T data, Node* next = nullptr, Node* prev = nullptr) : data(data), next(next), prev(prev) {}
    };

private:
    Node* head;
    Node* tail;
    UInt _length;

    void _forEachFromLast(Node* node, function<void(T&)>& closure)
    {
        if (!node)
        {
            return;
        }

        closure(node->data);

        _forEachFromLast(node->prev, closure);
    }

    // Comparador por defecto: orden ascendente.
    static bool ascending(const T& a, const T& b)
    {
        return a > b;
    }

    // Los algoritmos de ordenamiento reconectan los nodos usando solo `next`.
    // Esta funcion reconstruye `prev` y `tail` al terminar (un solo recorrido).
    void rebuildLinks()
    {
        Node* previous = nullptr;
        Node* current = head;

        while (current != nullptr)
        {
            current->prev = previous;
            previous = current;
            current = current->next;
        }

        tail = previous;
    }

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), _length(0) {}

    ~DoublyLinkedList()
    {
        Node* tmp = head;
        while (tmp != nullptr)
        {
            head = head->next;
            delete tmp;
            tmp = head;
        }
    }

    // Se elimina la copia por construcción
    DoublyLinkedList(const DoublyLinkedList&) = delete;

    // Se elimina la copia por asignación
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    // Permite construir una lista en base a las propiedades de otra.
    DoublyLinkedList(DoublyLinkedList&& other) noexcept
        : head(other.head), tail(other.tail), _length(other._length)
    {
        other.head = nullptr;
        other.tail = nullptr;
        other._length = 0;
    }

    // Permite transferir la informacion de una lista a la actual.
    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept
    {
        if (this != &other)
        {
            while (head != nullptr)
            {
                Node* tmp = head;
                head = head->next;
                delete tmp;
            }

            head = other.head;
            tail = other.tail;
            _length = other._length;

            other.head = nullptr;
            other.tail = nullptr;
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

    void append(T data, UInt atPosition)
    {
        if (atPosition > _length)
        {
            return;
        }

        if (atPosition == 0)
        {
            Node* newNode = new Node(data, head);

            if (head)
                head->prev = newNode;

            head = newNode;

            if (_length == 0)
                tail = head;

            _length++;
        }
        else if (atPosition == _length)
        {
            Node* newNode = new Node(data, nullptr, tail);

            if (tail)
                tail->next = newNode;

            tail = newNode;

            if (_length == 0)
                head = tail;

            _length++;
        }
        else
        {
            Node* tmp = head;

            for (UInt i = 1; i < atPosition; i++)
            {
                tmp = tmp->next;
            }

            Node* newNode = new Node(data, tmp->next, tmp);
            tmp->next = newNode;

            if (newNode->next)
                newNode->next->prev = newNode;

            _length++;
        }
    }

    void append(T data)
    {
        append(data, _length);
    }

    void replace(T newContent, UInt atPosition)
    {
        if (atPosition >= _length)
            return;

        Node* tmp = head;

        for (UInt i = 0; i < atPosition; i++)
        {
            tmp = tmp->next;
        }

        tmp->data = newContent;
    }

    optional<T> remove(UInt atPosition)
    {
        if (atPosition >= _length)
        {
            return {};
        }

        Node* tmp = head;

        if (atPosition == 0)
        {
            head = head->next;

            if (head)
                head->prev = nullptr;
        }
        else if (atPosition == _length - 1)
        {
            tmp = tail;
            tail = tail->prev;

            if (tail)
                tail->next = nullptr;
        }
        else
        {
            for (UInt i = 0; i < atPosition; i++)
            {
                tmp = tmp->next;
            }

            tmp->prev->next = tmp->next;

            if (tmp->next)
            {
                tmp->next->prev = tmp->prev;
            }
        }

        T value = tmp->data;

        delete tmp;
        _length--;

        if (_length == 0)
        {
            head = nullptr;
            tail = nullptr;
        }

        return value;
    }

    optional<T> removeFirst()
    {
        return remove(0);
    }

    optional<T> removeLast()
    {
        return remove(_length - 1);
    }

    optional<T> firstValue()
    {
        return value(0);
    }

    optional<T> value(UInt atPosition)
    {
        if (atPosition < _length)
        {
            Node* tmp = head;

            for (UInt i = 0; i < atPosition; i++)
            {
                tmp = tmp->next;
            }

            return tmp->data;
        }

        return {};
    }

    optional<T> lastValue()
    {
        return value(_length - 1);
    }

    // T : operator `==`
    bool contains(T data)
    {
        Node* tmp = head;

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

    // ------------------------------------------------------------------
    // ORDENAMIENTO
    //
    // El predicado recibe dos elementos (a, b) y debe retornar `true` cuando
    // `a` debe ir DESPUES de `b` (es decir, cuando hay que intercambiarlos).
    // Por defecto: `a > b`, lo que ordena de forma ascendente.
    //
    // Los tres algoritmos son ESTABLES (los elementos "iguales" conservan su
    // orden original) y reordenan los nodos cambiando enlaces: no copian datos.
    // Al terminar se actualizan `prev` y `tail`.
    // ------------------------------------------------------------------

    using Comparator = function<bool(const T&, const T&)>;

    // Ordenamiento por INSERCION (el mas recomendable): inserta cada nodo en
    // su lugar dentro de una sublista ya ordenada. Muy rapido si los datos ya
    // estan casi ordenados.
    // Tiempo: O(n^2) peor caso, O(n) mejor caso. Memoria extra: O(1).
    void insertionSort(Comparator predicate = &DoublyLinkedList::ascending)
    {
        if (!head || !head->next)
        {
            return;
        }

        Node* sorted = nullptr; // Cabeza de la sublista ya ordenada.
        Node* current = head;

        while (current != nullptr)
        {
            Node* next = current->next;

            if (sorted == nullptr || predicate(sorted->data, current->data))
            {
                current->next = sorted;
                sorted = current;
            }
            else
            {
                Node* scan = sorted;
                while (scan->next != nullptr && !predicate(scan->next->data, current->data))
                {
                    scan = scan->next;
                }

                current->next = scan->next;
                scan->next = current;
            }

            current = next;
        }

        head = sorted;
        rebuildLinks();
    }

    // Ordenamiento por SELECCION: busca el menor elemento de la parte no
    // ordenada y lo mueve al final de la parte ordenada.
    // Tiempo: O(n^2) siempre. Memoria extra: O(1).
    void selectionSort(Comparator predicate = &DoublyLinkedList::ascending)
    {
        if (!head || !head->next)
        {
            return;
        }

        Node* sortedHead = nullptr;
        Node* sortedTail = nullptr;

        while (head != nullptr)
        {
            Node* minPrev = nullptr;
            Node* min = head;

            Node* prev = head;
            for (Node* cur = head->next; cur != nullptr; prev = cur, cur = cur->next)
            {
                if (predicate(min->data, cur->data))
                {
                    min = cur;
                    minPrev = prev;
                }
            }

            // Desengancha el nodo de la lista pendiente.
            if (minPrev != nullptr)
            {
                minPrev->next = min->next;
            }
            else
            {
                head = min->next;
            }
            min->next = nullptr;

            // Lo agrega al final de la lista ordenada.
            if (sortedTail != nullptr)
            {
                sortedTail->next = min;
            }
            else
            {
                sortedHead = min;
            }
            sortedTail = min;
        }

        head = sortedHead;
        rebuildLinks();
    }

    // Ordenamiento BURBUJA: compara nodos vecinos e intercambia los que estan
    // fuera de orden; termina antes si en una pasada no hubo cambios.
    // Tiempo: O(n^2) peor caso, O(n) mejor caso. Memoria extra: O(1).
    void bubbleSort(Comparator predicate = &DoublyLinkedList::ascending)
    {
        if (!head || !head->next)
        {
            return;
        }

        Node* limit = nullptr; // Desde este nodo en adelante ya esta ordenado.
        bool swapped;

        do
        {
            swapped = false;
            Node* previous = nullptr;
            Node* current = head;

            while (current->next != limit)
            {
                Node* next = current->next;

                if (predicate(current->data, next->data))
                {
                    current->next = next->next;
                    next->next = current;

                    if (previous)
                    {
                        previous->next = next;
                    }
                    else
                    {
                        head = next;
                    }

                    previous = next;
                    swapped = true;
                }
                else
                {
                    previous = current;
                    current = next;
                }
            }

            limit = current;
        } while (swapped);

        rebuildLinks();
    }

    // Ordena usando insercion, el algoritmo recomendado para esta lista.
    void sort(Comparator predicate = &DoublyLinkedList::ascending)
    {
        insertionSort(predicate);
    }

    void forEachFromLast(function<void(T&)> closure)
    {
        _forEachFromLast(tail, closure);
    }

    void clear()
    {
        Node* tmp = head;
        while (tmp != nullptr)
        {
            head = head->next;
            delete tmp;
            tmp = head;
        }
    }
};