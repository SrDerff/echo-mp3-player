#pragma once
//#include <stdexcept>

using uint = unsigned int;

/**
 * @file Queue.h
 * @brief Cola (FIFO) generica implementada con lista enlazada simple.
 *
 * Estructura donde el primer elemento en entrar es el primero en salir.
 * Se utiliza en Playlist como "Up Next" (cola de reproduccion pendiente).
 * Mantiene punteros a ambos extremos (front y back) para operaciones O(1).
 */

template<typename T>
class Queue {
private:
    struct QueueNode {
        T data;
        QueueNode* next;
        QueueNode(T d) : data(d), next(nullptr) {}
    };
    QueueNode* frontNode;  ///< Puntero al frente (siguiente en salir).
    QueueNode* backNode;   ///< Puntero al final (ultimo en entrar).
    uint count;

public:
    Queue() : frontNode(nullptr), backNode(nullptr), count(0) {}

    ~Queue() {
        while (!isEmpty()) dequeue();
    }

    /**
     * @brief Encola un elemento al final.
     * @param value Dato de tipo T.
     * Si la cola esta vacia, front y back apuntan al nuevo nodo.
     */
    void enqueue(T value) {
        QueueNode* newNode = new QueueNode(value);
        if (isEmpty()) {
            frontNode = backNode = newNode;
        }
        else {
            backNode->next = newNode;  // El ultimo nodo apunta al nuevo.
            backNode = newNode;        // Back avanza al nuevo nodo.
        }
        count++;
    }

    /**
     * @brief Desencola el elemento del frente.
     * Libera la memoria del nodo frontal. Si queda vacia, back se anula.
     */
    void dequeue() {
        if (isEmpty()) return;
        QueueNode* temp = frontNode;
        frontNode = frontNode->next;  // El frente avanza al siguiente.
        if (frontNode == nullptr) backNode = nullptr;  // Cola vacia.
        delete temp;
        count--;
    }

    /**
     * @brief Consulta el elemento del frente sin eliminarlo.
     * @throw std::runtime_error Si la cola esta vacia.
     */
    T front() const {
        if (isEmpty()) throw std::runtime_error("Queue vacia");
        return frontNode->data;
    }

    T back() const {
        if (isEmpty()) throw std::runtime_error("Queue vacia");
        return backNode->data;
    }

    bool isEmpty() const { return frontNode == nullptr; }
    uint getSize() const { return count; }

    void clear() {
        while (!isEmpty()) dequeue();
    }
};