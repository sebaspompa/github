#ifndef ARBOL_BINARIO_HPP
#define ARBOL_BINARIO_HPP

#include <iostream>

// ---------- Nodo ----------
struct Nodo {
    int dato;
    Nodo* izquierda;
    Nodo* derecha;

    Nodo(int valor) {
        dato = valor;
        izquierda = nullptr;
        derecha = nullptr;
    }
};

// ---------- Operaciones del arbol ----------

// Inserta un valor (ignora duplicados)
inline Nodo* insertar(Nodo* raiz, int valor) {
    if (raiz == nullptr)
        return new Nodo(valor);

    if (valor < raiz->dato)
        raiz->izquierda = insertar(raiz->izquierda, valor);
    else if (valor > raiz->dato)
        raiz->derecha = insertar(raiz->derecha, valor);

    return raiz;
}

// Busca un valor en el arbol
inline bool buscar(Nodo* raiz, int valor) {
    if (raiz == nullptr)
        return false;

    if (raiz->dato == valor)
        return true;

    if (valor < raiz->dato)
        return buscar(raiz->izquierda, valor);

    return buscar(raiz->derecha, valor);
}

// Recorrido inorden (valores ordenados)
inline void mostrarInOrden(Nodo* raiz) {
    if (raiz == nullptr)
        return;

    mostrarInOrden(raiz->izquierda);
    std::cout << raiz->dato << " ";
    mostrarInOrden(raiz->derecha);
}

// Libera toda la memoria del arbol
inline void liberarArbol(Nodo* raiz) {
    if (raiz == nullptr)
        return;

    liberarArbol(raiz->izquierda);
    liberarArbol(raiz->derecha);
    delete raiz;
}

// ---------- Menu interactivo ----------
inline void ejecutarMenu() {
    Nodo* raiz = nullptr;
    int opcion;
    int valor;

    do {
        std::cout << "\n--- ARBOL BINARIO DE BUSQUEDA ---\n";
        std::cout << "1. Insertar un valor\n";
        std::cout << "2. Buscar un valor\n";
        std::cout << "3. Mostrar valores ordenados\n";
        std::cout << "4. Salir\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
        case 1:
            std::cout << "Ingrese el valor a insertar: ";
            std::cin >> valor;
            raiz = insertar(raiz, valor);
            std::cout << "Valor procesado.\n";
            break;

        case 2:
            std::cout << "Ingrese el valor a buscar: ";
            std::cin >> valor;

            if (buscar(raiz, valor))
                std::cout << "El valor " << valor
                          << " SI se encuentra en el arbol.\n";
            else
                std::cout << "El valor " << valor
                          << " NO se encuentra en el arbol.\n";
            break;

        case 3:
            std::cout << "Valores ordenados: ";
            mostrarInOrden(raiz);
            std::cout << std::endl;
            break;

        case 4:
            std::cout << "Fin del programa.\n";
            break;

        default:
            std::cout << "Opcion incorrecta.\n";
        }

    } while (opcion != 4);

    liberarArbol(raiz);
}

#endif // ARBOL_BINARIO_HPP
