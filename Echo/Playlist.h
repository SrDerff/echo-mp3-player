#pragma once
#include "DoublyLinkedList.h"
#include "Song.h"
#include "GestorAudio.h"
#include "Stack.h"
#include "Queue.h"
#include <iostream>

/**
 * @file Playlist.h
 * @brief Entidad que representa una lista de reproduccion.
 *
 * Combina una DoublyLinkedList de canciones con un reproductor (GestorAudio),
 * un historial de navegacion (Stack) y una cola de siguientes (Queue).
 * Gestiona la logica de reproduccion: play, pause, next, previous, loop.
 */

class Playlist {
private:
    string name;
    DoublyLinkedList<Song> songs;  ///< Coleccion de canciones de la playlist.
    Node<Song>* currentNode;       ///< Puntero al nodo de la cancion actual.
    GestorAudio audio;             ///< Instancia del reproductor multimedia.
    bool loop;                     ///< Si true, al terminar la lista vuelve al inicio.

    Stack<Song*> history;          ///< Historial de canciones reproducidas (LIFO).
    Queue<Song> upNext;            ///< Cola de canciones pendientes (FIFO).

public:
    Playlist(string name = "Nueva Playlist") : name(name), currentNode(nullptr), loop(false) {}

    ~Playlist() {
        audio.cerrar();  // Asegura liberacion del dispositivo de audio.
    }

    void setName(const string& n) { name = n; }
    string getName() const { return name; }

    void addSong(const Song& song) {
        songs.insertAtTail(song);
        if (currentNode == nullptr) currentNode = songs.getHead();  // Auto-seleccionar primera cancion.
    }

    bool removeSong(const Song& song) {
        if (currentNode != nullptr && currentNode->getValue() == song) {
            audio.detener();
            audio.cerrar();
            currentNode = nullptr;
        }
        return songs.removeNode(song);
    }

    bool removeAt(uint index) {
        if (currentNode != nullptr) {
            Song s = songs.getAt(index);
            if (currentNode->getValue().getName() == s.getName()) {
                audio.detener();
                audio.cerrar();
                currentNode = nullptr;
            }
        }
        return songs.removeAt(index);
    }

    // --- REPRODUCCION ---

    /**
     * @brief Inicia o reanuda la reproduccion de la cancion actual.
     * Si la cancion actual no esta cargada en el GestorAudio, la carga desde su ruta.
     * Si ya esta cargada, simplemente reproduce (o reanuda si estaba pausada).
     */
    void play() {
        if (currentNode == nullptr && songs.getHead() != nullptr)
            currentNode = songs.getHead();

        if (currentNode != nullptr) {
            Song s = currentNode->getValue();
            if (!audio.estaCargado() || audio.getActual() != s.getSource()) {
                audio.cerrar();
                if (audio.cargar(s.getSource())) {
                    audio.reproducir();
                }
                else {
                    cout << "Error al cargar: " << s.getSource() << "\n";
                }
            }
            else {
                audio.reproducir();
            }
        }
    }

    void pause() { audio.pausar(); }
    void stop() { audio.detener(); }

    /**
     * @brief Avanza a la siguiente cancion.
     *
     * Logica de prioridad:
     * 1. Si hay cola upNext, reproduce la primera cancion de la cola.
     * 2. Si no hay cola, avanza al siguiente nodo en la lista.
     * 3. Si llego al final y loop esta activo, vuelve al head.
     *
     * Antes de avanzar, guarda la cancion actual en el historial (Stack).
     */
    void next() {
        if (currentNode == nullptr) return;

        history.push(&(currentNode->getValue()));  // Guardamos en historial para poder volver.
        audio.detener();

        if (!upNext.isEmpty()) {
            Song nextSong = upNext.front();
            upNext.dequeue();
            Node<Song>* found = findSongNode(nextSong);
            if (found != nullptr) {
                currentNode = found;
                play();
                return;
            }
        }

        currentNode = currentNode->next;
        if (currentNode == nullptr) {
            if (loop && songs.getHead() != nullptr) {
                currentNode = songs.getHead();
                play();
            }
        }
        else {
            play();
        }
    }

    /**
     * @brief Retrocede a la cancion anterior.
     *
     * Si el historial (Stack) no esta vacio, vuelve a la ultima cancion reproducida.
     * Si no hay historial, retrocede al nodo previo en la lista doblemente enlazada.
     */
    void previous() {
        if (history.isEmpty()) {
            if (currentNode != nullptr && currentNode->prev != nullptr) {
                audio.detener();
                currentNode = currentNode->prev;
                play();
            }
        }
        else {
            audio.detener();
            Song* last = history.top();
            history.pop();
            Node<Song>* found = findSongNode(*last);
            if (found != nullptr) {
                currentNode = found;
                play();
            }
        }
    }

    // --- COLA UP NEXT ---
    void addToQueue(const Song& song) { upNext.enqueue(song); }
    void clearQueue() { upNext.clear(); }
    uint getQueueSize() const { return upNext.getSize(); }

    // --- HISTORIAL ---
    void clearHistory() { history.clear(); }
    uint getHistorySize() const { return history.size(); }

    // --- CONTROLES ---
    void toggleLoop() { loop = !loop; }
    bool isLooping() const { return loop; }

    void setVolume(int vol) { audio.setVolumen(vol); }
    int getVolume() const { return audio.getVolumen(); }

    float getCurrentPosition() { return audio.getPosicion(); }
    float getCurrentDuration() { return audio.getDuracion(); }

    Song* getCurrentSong() const {
        if (currentNode == nullptr) return nullptr;
        return &(currentNode->getValue());
    }

    void seekTo(float seconds) { audio.setPosicion(seconds); }

    bool isPlaying() { return audio.estaReproduciendo(); }
    bool isPaused() const { return audio.estaPausado(); }

    uint getSize() const { return songs.getSize(); }
    bool isEmpty() const { return songs.isEmpty(); }

    void clear() {
        audio.cerrar();
        songs.clear();
        currentNode = nullptr;
        history.clear();
        upNext.clear();
    }

    Song getSongAt(uint index) const {
        return songs.getAt(index);
    }

    void printPlaylist() const {
        cout << "\n=== " << name << " ===\n";
        Node<Song>* curr = songs.getHead();
        uint i = 0;
        while (curr != nullptr) {
            cout << (curr == currentNode ? " > " : "   ") << i++ << ". ";
            cout << curr->getValue().getName() << " - " << curr->getValue().getAuthor()
                << " [" << curr->getValue().getDuration() << "s]\n";
            curr = curr->next;
        }
        if (songs.isEmpty()) cout << " (Vacia)\n";

        if (!upNext.isEmpty()) {
            cout << "\n [En cola: " << upNext.getSize() << " cancion(es)]\n";
        }
        if (!history.isEmpty()) {
            cout << " [Historial: " << history.size() << " cancion(es)]\n";
        }
        cout << "==================\n";
    }

private:
    /**
     * @brief Busca un nodo especifico en la lista de canciones.
     * @param song Cancion a buscar (comparacion por operator==).
     * @return Puntero al nodo o nullptr.
     */
    Node<Song>* findSongNode(const Song& song) {
        Node<Song>* curr = songs.getHead();
        while (curr != nullptr) {
            if (curr->getValue() == song) return curr;
            curr = curr->next;
        }
        return nullptr;
    }
};