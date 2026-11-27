#pragma once

#include <functional>
#include <utility>
#include <vector>

using std::vector;

struct HeapSort
{
private:
    static void siftDown(auto &arr, int i, int size, const auto &predicate)
    {
        int target = i;
        const int left = 2 * i + 1;
        const int right = 2 * i + 2;

        if (left < size && predicate(arr[target], arr[left]))
        {
            target = left;
        }

        if (right < size && predicate(arr[target], arr[right]))
        {
            target = right;
        }

        if (target != i)
        {
            std::swap(arr[i], arr[target]);
            siftDown(arr, target, size, predicate);
        }
    }

    static void buildHeap(auto &arr, const auto &predicate)
    {
        const int n = static_cast<int>(arr.size());

        for (int i = n / 2 - 1; i >= 0; --i)
        {
            siftDown(arr, i, n, predicate);
        }
    }

    static void sortImpl(auto &arr, const auto &predicate)
    {
        const int n = static_cast<int>(arr.size());

        buildHeap(arr, predicate);

        for (int end = n - 1; end > 0; --end)
        {
            std::swap(arr[0], arr[end]);
            siftDown(arr, 0, end, predicate);
        }
    }

public:
    // Usa Max-Heap (a < b): ordena de menor a mayor (ascendente)
    template <typename T>
    static void sortAscending(vector<T> &arr, auto predicate = std::less<>{})
    {
        sortImpl(arr, predicate);
    }

    // Usa Min-Heap (a > b): ordena de mayor a menor (descendente)
    template <typename T>
    static void sortDescending(vector<T> &arr, auto predicate = std::greater<>{})
    {
        sortImpl(arr, predicate);
    }
};
