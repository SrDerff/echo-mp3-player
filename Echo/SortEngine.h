#pragma once
#include <vector>
#include <functional>

using namespace std;

using uint = unsigned int;

/**
 * @file SortEngine.h
 * @brief Motor de ordenamiento generico con Quicksort recursivo.
 *
 * Implementa el algoritmo Quicksort (divide y venceras) sobre un vector de punteros.
 * Usa una lambda como comparador, permitiendo ordenar por cualquier criterio
 * (nombre, duracion, artista) sin modificar las clases de datos.
 * Complejidad promedio: O(n log n). Peor caso: O(n^2).
 */

template<typename T>
class SortEngine {
private:
    /**
     * @brief Particiona el subvector usando el ultimo elemento como pivote.
     * @param arr Vector de punteros a ordenar.
     * @param low Indice inferior del subvector.
     * @param high Indice superior del subvector (pivote).
     * @param comp Lambda de comparacion. Devuelve true si el primer arg va antes que el segundo.
     * @return Posicion final del pivote despues de particionar.
     *
     * Recorre el subvector una vez (j de low a high-1). Si arr[j] debe ir antes que el pivote
     * segun comp, lo intercambia con arr[i+1] (donde i marca la frontera de menores).
     */
    static int partition(vector<T*>& arr, int low, int high, function<bool(T*, T*)> comp) {
        T* pivot = arr[high];  // Elegimos el ultimo elemento como pivote.
        int i = low - 1;       // i marca el final de la zona de "menores al pivote".
        for (int j = low; j < high; j++) {
            if (comp(arr[j], pivot)) {
                i++;
                swap(arr[i], arr[j]);  // Intercambio estandar de la STL.
            }
        }
        swap(arr[i + 1], arr[high]);  // Colocamos el pivote en su posicion correcta.
        return i + 1;
    }

    /**
     * @brief Quicksort recursivo.
     * @param low, high Limites del subvector actual.
     *
     * Caso base: low < high (si hay 0 o 1 elementos, ya esta ordenado).
     * Recursivo: particiona, luego ordena recursivamente izquierda y derecha.
     */
    static void quickSortRec(vector<T*>& arr, int low, int high, function<bool(T*, T*)> comp) {
        if (low < high) {
            int pi = partition(arr, low, high, comp);
            quickSortRec(arr, low, pi - 1, comp);   // Ordenar subvector izquierdo.
            quickSortRec(arr, pi + 1, high, comp);  // Ordenar subvector derecho.
        }
    }

public:
    /**
     * @brief Ordena un vector de punteros usando Quicksort.
     * @param arr Vector a ordenar (pasado por referencia para modificarlo in-situ).
     * @param comp Lambda comparadora. Ejemplo: [](Song* a, Song* b){ return a->getName() < b->getName(); }
     */
    static void quickSort(vector<T*>& arr, function<bool(T*, T*)> comp) {
        if (arr.size() > 1)
            quickSortRec(arr, 0, arr.size() - 1, comp);
    }
};