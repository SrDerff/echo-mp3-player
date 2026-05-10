#pragma once
#include <stdexcept>

/**
 * @file Stack.h
 * @brief Pila (LIFO) generica implementada con lista enlazada simple.
 *
 * Estructura fundamental donde el ultimo elemento en entrar es el primero en salir.
 * Se utiliza en Playlist para mantener el historial de canciones reproducidas,
 * permitiendo la funcion "anterior" (backtracking).
 */

template<typename T>
class Stack {
private:
    /**
     * @brief Nodo interno de la pila. Se mantiene privado para ocultar la implementacion.
     */
    struct StackNode {
        T data;           ///< Dato almacenado.
        StackNode* next;  ///< Puntero al nodo inferior en la pila.
        StackNode(T d) : data(d), next(nullptr) {}
    };
    StackNode* topNode;   ///< Puntero al tope de la pila. nullptr si esta vacia.
    uint count;           ///< Numero de elementos.

public:
    Stack() : topNode(nullptr), count(0) {}

    /**
     * @brief Destructor. Desapila todos los elementos para liberar memoria.
     */
    ~Stack() {
        while (!isEmpty()) pop();
    }

    /**
     * @brief Apila un elemento en el tope.
     * @param value Dato de tipo T. Se crea un nuevo nodo que referencia al antiguo tope.
     * Complejidad: O(1).
     */
    void push(T value) {
        StackNode* newNode = new StackNode(value);
        newNode->next = topNode;  // El nuevo nodo descansa sobre el tope actual.
        topNode = newNode;        // El tope sube al nuevo nodo.
        count++;
    }

    /**
     * @brief Desapila el elemento del tope.
     * Elimina el nodo superior y decrementa el contador. Seguro llamar si isEmpty() es false.
     */
    void pop() {
        if (isEmpty()) return;
        StackNode* temp = topNode;
        topNode = topNode->next;  // El tope baja al siguiente nodo.
        delete temp;              // Liberamos la memoria del antiguo tope.
        count--;
    }

    /**
     * @brief Consulta el elemento del tope sin eliminarlo.
     * @return Copia del dato en el tope.
     * @throw std::runtime_error Si la pila esta vacia.
     */
    T top() const {
        if (isEmpty()) throw std::runtime_error("Stack vacio");
        return topNode->data;
    }

    bool isEmpty() const { return topNode == nullptr; }
    uint size() const { return count; }

    void clear() {
        while (!isEmpty()) pop();
    }
};