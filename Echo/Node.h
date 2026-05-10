#pragma once

/**
 * @file Node.h
 * @brief Plantilla de nodo doblemente enlazado para estructuras de datos genericas.
 *
 * Esta clase template es la unidad basica de almacenamiento utilizada por
 * DoublyLinkedList y CircularDoublyLinkedList. Al ser template <typename T>,
 * permite almacenar cualquier tipo de dato sin reescribir codigo (polimorfismo parametrico).
 */

template<typename T>
class Node
{
private:
    T value;            ///< Dato almacenado en el nodo. T puede ser cualquier tipo (primitivo o clase).
public:
    Node<T>* next;      ///< Puntero al siguiente nodo en la secuencia.
    Node<T>* prev;      ///< Puntero al nodo anterior. Habilita recorrido bidireccional.

    /**
     * @brief Constructor por defecto.
     * Inicializa el valor con el constructor por defecto de T (T()) y punteros nulos.
     * Se usa cuando se necesita un nodo temporal antes de asignarle un valor real.
     */
    Node() : value(T()), next(nullptr), prev(nullptr) {}

    /**
     * @brief Constructor parametrizado.
     * @param value Valor de tipo T que se almacenara en el nodo.
     * Recibe una copia del dato (paso por valor) o una referencia dependiendo de como se instancie.
     * Los punteros next y prev se inicializan en nullptr indicando que el nodo no esta enlazado.
     */
    Node(T value) : value(value), next(nullptr), prev(nullptr) {}

    /**
     * @brief Obtiene el valor almacenado.
     * @return Copia del valor de tipo T. Si T es grande, considerar retornar const T&.
     */
    T getValue() const { return value; }

    /**
     * @brief Modifica el valor almacenado.
     * @param value Nuevo valor de tipo T a almacenar.
     */
    void setValue(T value) { this->value = value; }
};