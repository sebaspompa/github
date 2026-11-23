#pragma once

#include <functional>
#include <optional>
#include <cstddef>

using std::optional;
using std::nullopt;
using std::function;
using std::size_t;
using UInt = unsigned int;

template <typename T>
class LinkedList
{
public:
    struct Node
    {
        T data;
        Node* next;

        Node(T data, Node* next = nullptr) : data(data), next(next) {}
    };

public:
    Node* head;
    UInt _length;

public:
    LinkedList() : head(nullptr), _length(0) {}

    ~LinkedList()
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
    LinkedList(const LinkedList&) = delete;

    // Se elimina la copia por asignación
    LinkedList& operator=(const LinkedList&) = delete;

    // Permite construir una lista en base a las propiedades de otra.
    LinkedList(LinkedList&& other) noexcept : head(other.head), _length(other._length)
    {
        other.head = nullptr;
        other._length = 0;
    }

    // Permite transferir la informacion de una lista a la actual.
    LinkedList& operator=(LinkedList&& other) noexcept
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
            _length = other._length;

            other.head = nullptr;
            other._length = 0;
        }

        return *this;
    }

    UInt length() const { return _length; }

    bool isEmpty() const { return _length == 0; }

    void append(T data, UInt atPosition)
    {
        if (atPosition > _length)
        {
            return;
        }

        if (atPosition == 0)
        {
            Node* newNode = new Node(data, head);
            head = newNode;
            _length++;
        }
        else
        {
            Node* tmp = head;
            for (UInt i = 1; i < atPosition; i++)
            {
                tmp = tmp->next;
            }
            Node* newNode = new Node(data, tmp->next);
            tmp->next = newNode;
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
        {
            return;
        }

        Node* tmp = head;
        for (UInt i = 0; i < atPosition; i++)
        {
            tmp = tmp->next;
        }
        tmp->data = newContent;
    }

    optional<T> remove(UInt atPosition)
    {
        if (atPosition >= _length || atPosition < 0)
        {
            return nullopt;
        }

        Node* tmp = head;

        if (atPosition == 0)
        {
            head = head->next;
        }
        else
        {
            Node* prev = nullptr;
            for (UInt i = 0; i < atPosition; i++)
            {
                prev = tmp;
                tmp = tmp->next;
            }
            prev->next = tmp->next;
        }

        T value = tmp->data;
        delete tmp;
        _length--;
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
        size_t i = 0;
        Node* tmp = head;
        while (tmp != nullptr)
        {
            if (i == atPosition)
            {
                return tmp->data;
            }

            tmp = tmp->next;
            i++;
        }

        return nullopt;
    }

    optional<T> lastValue()
    {
        return value(_length - 1);
    }

    // T : operator ==
    bool contains(const T& data) const
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
    // ------------------------------------------------------------------

    using Comparator = function<bool(const T&, const T&)>;

    // Ordenamiento por INSERCION. Es el mas recomendable para esta lista:
    // inserta cada nodo en su lugar dentro de una lista ya ordenada, sin
    // necesitar acceso por indice. Muy rapido si los datos ya estan casi ordenados.
    // Tiempo: O(n^2) peor caso, O(n) mejor caso. Memoria extra: O(1).
    void insertionSort(Comparator predicate = &LinkedList::ascending)
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
                // Va antes que todos los de la sublista ordenada.
                current->next = sorted;
                sorted = current;
            }
            else
            {
                // Avanza hasta el ultimo nodo que NO debe ir despues de `current`.
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
    }

    // Ordenamiento por SELECCION. Busca el menor elemento de la parte no
    // ordenada y lo mueve al final de la parte ordenada.
    // Tiempo: O(n^2) siempre. Memoria extra: O(1).
    void selectionSort(Comparator predicate = &LinkedList::ascending)
    {
        if (!head || !head->next)
        {
            return;
        }

        Node* sortedHead = nullptr;
        Node* sortedTail = nullptr;

        while (head != nullptr)
        {
            // Busca el primer nodo "menor" de lo que queda sin ordenar.
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

            // Lo desengancha de la lista pendiente.
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
    }

    // Ordenamiento BURBUJA. Compara nodos vecinos e intercambia los que estan
    // fuera de orden; termina antes si en una pasada no hubo cambios.
    // Tiempo: O(n^2) peor caso, O(n) mejor caso. Memoria extra: O(1).
    void bubbleSort(Comparator predicate = &LinkedList::ascending)
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
    }

    void mergeSort(Comparator predicate = &LinkedList::ascending)
    {
        if (!head || !head->next)
        {
            return;
        }

        head = _mergeSort(head, predicate);
    }

private:
    static Node* _merge(Node* left, Node* right, const Comparator& predicate)
    {
        Node* result = nullptr;
        Node** last = &result;

        while (left != nullptr && right != nullptr)
        {
            if (predicate(left->data, right->data))
            {
                *last = right;
                right = right->next;
            }
            else
            {
                *last = left;
                left = left->next;
            }
            last = &(*last)->next;
        }

        *last = (left != nullptr) ? left : right;
        return result;
    }

    static Node* _mergeSort(Node* node, const Comparator& predicate)
    {
        if (node == nullptr || node->next == nullptr)
        {
            return node;
        }

        Node* slow = node;
        Node* fast = node->next;
        while (fast != nullptr && fast->next != nullptr)
        {
            slow = slow->next;
            fast = fast->next->next;
        }

        Node* right = slow->next;
        slow->next = nullptr;

        Node* sortedLeft = _mergeSort(node, predicate);
        Node* sortedRight = _mergeSort(right, predicate);

        return _merge(sortedLeft, sortedRight, predicate);
    }

public:
    // Se mantiene por compatibilidad con el codigo existente.
    // Usa insercion, que es el algoritmo recomendado para esta lista.
    void sort(Comparator predicate = &LinkedList::ascending)
    {
        mergeSort(predicate);
    }

    // Retorna una lista de todos los elementos que satisfacen el predicado dado.
    LinkedList<T> filter(function<bool(T)> predicate)
    {
        LinkedList<T> items;

        Node* tmp = head;
        while (tmp != nullptr)
        {
            if (predicate(tmp->data))
            {
                items.append(tmp->data);
            }

            tmp = tmp->next;
        }

        return items;
    }

    void forEach(function<void(T)> closure) const
    {
        Node* tmp = head;
        while (tmp != nullptr)
        {
            closure(tmp->data);
            tmp = tmp->next;
        }
    }

    void map(function<void(T&)> closure)
    {
        Node* tmp = head;
        while (tmp != nullptr)
        {
            closure(tmp->data);
            tmp = tmp->next;
        }
    }

private:
    // Comparador por defecto: orden ascendente.
    static bool ascending(const T& a, const T& b)
    {
        return a > b;
    }

    // *** Funcion auxiliar para `max()` y `min()` ***
    // Retorna el elemento maximo o minimo de la lista usando el predicado dado como comparador entre elementos.
    optional<T> minMax(function<bool(T, T)> predicate)
    {
        if (head == nullptr)
        {
            return nullopt;
        }

        Node* tmp = head;
        T value = head->data;
        while (tmp != nullptr)
        {
            if (predicate(value, tmp->data))
            {
                value = tmp->data;
            }

            tmp = tmp->next;
        }

        return value;
    }
};
