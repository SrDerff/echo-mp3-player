#pragma once
#include <map>
#include <vector>
#include <set>
#include <functional>
#include <string>

using namespace std;

/**
 * @file Graph.h
 * @brief Grafo no dirigido generico con DFS recursivo.
 *
 * Representa relaciones entre entidades (ej: artistas relacionados por genero).
 * Usa un mapa de listas de adyacencia (map<T, vector<T>>) donde T debe ser
 * comparable (operator<) para poder usarse como clave en std::map.
 * El recorrido DFS recursivo permite explorar toda una componente conexa.
 */

template<typename T>
class Graph {
private:
    map<T, vector<T>> adjList;  ///< Mapa de adyacencia. Clave = nodo, Valor = vecinos.

    /**
     * @brief DFS recursivo interno.
     * @param node Nodo actual a visitar.
     * @param visit Funcion lambda que se ejecuta sobre cada nodo descubierto.
     * @param visited Conjunto de nodos ya visitados (evita ciclos infinitos).
     *
     * Recursividad: visita el nodo actual, luego llama recursivamente a cada vecino no visitado.
     * Caso base: el nodo ya fue visitado (no hace nada) o no tiene vecinos.
     */
    void dfsRec(const T& node, function<void(const T&)> visit, set<T>& visited) const {
        if (visited.find(node) != visited.end()) return;  // Ya visitado: caso base/corte.
        visited.insert(node);
        visit(node);  // Ejecutamos la accion personalizada (lambda).

        auto it = adjList.find(node);
        if (it != adjList.end()) {
            for (const auto& neighbor : it->second) {
                dfsRec(neighbor, visit, visited);
            }
        }
    }

public:
    /**
     * @brief Agrega una arista entre dos nodos.
     * @param from Nodo origen.
     * @param to Nodo destino.
     * @param weight Peso (no usado en esta implementacion simplificada, reservado para futuro).
     * Como es no dirigido, la relacion es simetrica (se agrega en ambas direcciones).
     */
    void addEdge(const T& from, const T& to, int weight = 1) {
        adjList[from].push_back(to);
        adjList[to].push_back(from);
    }

    /**
     * @brief Ejecuta un recorrido en profundidad (DFS) desde un nodo inicial.
     * @param start Nodo donde comenzar el recorrido.
     * @param visit Lambda con firma void(const T&). Se invoca por cada nodo alcanzable.
     *
     * Es const porque no modifica la estructura del grafo (solo crea un set local temporal).
     */
    void dfs(const T& start, function<void(const T&)> visit) const {
        set<T> visited;
        dfsRec(start, visit, visited);
    }

    /**
     * @brief Verifica si un nodo existe en el grafo.
     * @param node Nodo a verificar.
     * @return true si el nodo tiene al menos una arista registrada.
     * Es const porque solo consulta el mapa.
     */
    bool hasNode(const T& node) const {
        return adjList.find(node) != adjList.end();
    }

    vector<T> getNeighbors(const T& node) const {
        auto it = adjList.find(node);
        if (it != adjList.end()) return it->second;
        return {};
    }

    void clear() { adjList.clear(); }
};