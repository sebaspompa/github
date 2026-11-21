#include <iostream>
using namespace std;


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


Nodo* insertar(Nodo* raiz, int valor) {
    
    if (raiz == nullptr)
        return new Nodo(valor);

    
    if (valor < raiz->dato)
        raiz->izquierda = insertar(raiz->izquierda, valor);

  
    else if (valor > raiz->dato)
        raiz->derecha = insertar(raiz->derecha, valor);


    return raiz;
}


bool buscar(Nodo* raiz, int valor) {
  
    if (raiz == nullptr)
        return false;

    
    if (raiz->dato == valor)
        return true;

    
    if (valor < raiz->dato)
        return buscar(raiz->izquierda, valor);

  
    return buscar(raiz->derecha, valor);
}


void mostrarInOrden(Nodo* raiz) {
    if (raiz == nullptr)
        return;

    mostrarInOrden(raiz->izquierda);
    cout << raiz->dato << " ";
    mostrarInOrden(raiz->derecha);
}

void liberarArbol(Nodo* raiz) {
    if (raiz == nullptr)
        return;

    liberarArbol(raiz->izquierda);
    liberarArbol(raiz->derecha);
    delete raiz;
}

int main() {
    Nodo* raiz = nullptr;
    int opcion;
    int valor;

    do {
        cout << "\n--- ARBOL BINARIO DE BUSQUEDA ---\n";
        cout << "1. Insertar un valor\n";
        cout << "2. Buscar un valor\n";
        cout << "3. Mostrar valores ordenados\n";
        cout << "4. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
        case 1:
            cout << "Ingrese el valor a insertar: ";
            cin >> valor;
            raiz = insertar(raiz, valor);
            cout << "Valor procesado.\n";
            break;

        case 2:
            cout << "Ingrese el valor a buscar: ";
            cin >> valor;

            if (buscar(raiz, valor))
                cout << "El valor " << valor
                << " SI se encuentra en el arbol.\n";
            else
                cout << "El valor " << valor
                << " NO se encuentra en el arbol.\n";
            break;

        case 3:
            cout << "Valores ordenados: ";
            mostrarInOrden(raiz);
            cout << endl;
            break;

        case 4:
            cout << "Fin del programa.\n";
            break;

        default:
            cout << "Opcion incorrecta.\n";
        }

    } while (opcion != 4);

    liberarArbol(raiz);
    return 0;
}