#pragma once
#include "DoublyLinkedList.h"
#include <vector>
#include <string>
#include <functional>

using namespace std;

/**
 * @file SearchFilter.h
 * @brief Filtro de busqueda generico con lambdas para listas doblemente enlazadas.
 *
 * Permite filtrar elementos de una DoublyLinkedList aplicando predicados (lambdas).
 * La funcion searchByString abstrae la busqueda case-insensitive por cualquier campo
 * de un objeto (nombre, autor, genero) usando un getter lambda.
 */

template<typename T>
class SearchFilter {
public:
    using Predicate = function<bool(const T&)>;  ///< Alias de tipo para legibilidad.

    /**
     * @brief Filtra elementos que cumplan un predicado.
     * @param list Lista doblemente enlazada a recorrer.
     * @param pred Lambda que recibe const T& y devuelve bool.
     * @return Vector de punteros a los elementos que satisfacen el predicado.
     */
    static vector<T*> filter(DoublyLinkedList<T>& list, Predicate pred) {
        vector<T*> results;
        Node<T>* curr = list.getHead();
        while (curr != nullptr) {
            if (pred(curr->getValue())) {
                results.push_back(&(curr->getValue()));
            }
            curr = curr->next;
        }
        return results;
    }

    /**
     * @brief Busqueda case-insensitive por subcadena en un campo especifico.
     * @param list Lista a buscar.
     * @param getter Lambda que extrae el string a comparar desde el objeto T.
     * @param query Texto buscado (se convierte a minusculas internamente).
     * @return Vector de punteros a coincidencias.
     *
     * Ejemplo de uso: buscar canciones por nombre.
     * getter = [](const Song& s){ return s.getName(); }
     */
    static vector<T*> searchByString(DoublyLinkedList<T>& list,
        function<string(const T&)> getter,
        const string& query) {
        string lowerQuery = toLowerStr(query);
        return filter(list, [&](const T& item) {
            return toLowerStr(getter(item)).find(lowerQuery) != string::npos;
            });
    }

private:
    /**
     * @brief Convierte una cadena a minusculas.
     * @param str Cadena pasada por valor (copia) para no modificar la original.
     * @return Cadena en minusculas.
     * tolower requiere cast a unsigned char para comportamiento definido con caracteres negativos.
     */
    static string toLowerStr(string str) {
        for (char& c : str) c = tolower(static_cast<unsigned char>(c));
        return str;
    }
};