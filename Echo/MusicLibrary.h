#pragma once
#include "Playlist.h"
#include "BinarySearchTree.h"
#include "Tree.h"
#include "Graph.h"
#include "CircularDoublyLinkedList.h"
#include "SortEngine.h"
#include "Artist.h"
#include "Album.h"
#include <vector>

using namespace std;
using uint = unsigned int;

class MusicLibrary {
private:
    int currentPlaylistIndex;
    vector<Playlist> playlists;
    DoublyLinkedList<Song> allSongs;
    BinarySearchTree<Song*> songByNameTree;   
    Trie<Song*> songTrie;
    Graph<string> artistGraph;
    vector<Artist> artists;
    vector<Album> albums;
    CircularDoublyLinkedList<Song> playbackHistory;
    Stack<Song> sessionHistory;

    static string toLowerStr(string str) {
        for (char& c : str) c = tolower(static_cast<unsigned char>(c));
        return str;
    }

    vector<Song*> getAllSongPtrs() {
        vector<Song*> ptrs;
        Node<Song>* curr = allSongs.getHead();
        while (curr != nullptr) {
            ptrs.push_back(&(curr->getValue()));
            curr = curr->next;
        }
        return ptrs;
    }

public:
    MusicLibrary() : currentPlaylistIndex(-1),
        songByNameTree([](Song* a, Song* b) { return a->getName() < b->getName(); }) {
    }

    
    void addPlaylist(const string& name) {
        playlists.emplace_back(name);
        if (currentPlaylistIndex == -1) currentPlaylistIndex = 0;
    }

    void removePlaylist(int index) {
        if (index >= 0 && index < (int)playlists.size()) {
            if (currentPlaylistIndex == index) currentPlaylistIndex = -1;
            playlists.erase(playlists.begin() + index);
            if (currentPlaylistIndex >= (int)playlists.size())
                currentPlaylistIndex = playlists.empty() ? -1 : 0;
        }
    }

    Playlist* getPlaylist(int index) {
        if (index >= 0 && index < (int)playlists.size()) return &playlists[index];
        return nullptr;
    }

    Playlist* getCurrentPlaylist() {
        return getPlaylist(currentPlaylistIndex);
    }

    void setCurrentPlaylist(int index) {
        if (index >= 0 && index < (int)playlists.size()) {
            if (currentPlaylistIndex != -1 && currentPlaylistIndex != index) {
                playlists[currentPlaylistIndex].stop();
            }
            currentPlaylistIndex = index;
        }
    }

    int getPlaylistCount() const { return playlists.size(); }
    int getCurrentIndex() const { return currentPlaylistIndex; }

    void addSongToLibrary(const Song& song) {
        allSongs.insertAtTail(song);
        Node<Song>* tail = allSongs.getTail();
        Song* ptr = &(tail->getValue());

        songByNameTree.insert(ptr);
        songTrie.insert(toLowerStr(ptr->getName()), ptr);

        bool foundArtist = false;
        for (auto& artist : artists) {
            if (artist.getName() == ptr->getAuthor()) {
                artist.addSong(*ptr);
                foundArtist = true;
                break;
            }
        }
        if (!foundArtist) {
            Artist newArtist(ptr->getAuthor(), "");
            newArtist.addSong(*ptr);
            artists.push_back(newArtist);
        }

        bool foundAlbum = false;
        for (auto& album : albums) {
            if (album.getTitle() == ptr->getGenre() && album.getArtistName() == ptr->getAuthor()) {
                album.addTrack(*ptr);
                foundAlbum = true;
                break;
            }
        }
        if (!foundAlbum) {
            Album newAlbum(ptr->getGenre(), ptr->getAuthor(), 2024);
            newAlbum.addTrack(*ptr);
            albums.push_back(newAlbum);
        }
    }

    void removeSongFromLibrary(const Song& song) {
        allSongs.removeNode(song);
    }

    DoublyLinkedList<Song>* getAllSongs() { return &allSongs; }

    void addSongToPlaylist(int playlistIndex, const Song& song) {
        Playlist* p = getPlaylist(playlistIndex);
        if (p) p->addSong(song);
    }

    vector<Song*> getSongsSortedByName() {
        vector<Song*> all = getAllSongPtrs();
        SortEngine<Song>::quickSort(all, [](Song* a, Song* b) {
            return a->getName() < b->getName();
            });
        return all;
    }

    vector<Song*> getSongsSortedByDuration() {
        vector<Song*> all = getAllSongPtrs();
        SortEngine<Song>::quickSort(all, [](Song* a, Song* b) {
            return a->getDuration() < b->getDuration();
            });
        return all;
    }

    vector<Song*> predictiveSearch(const string& prefix) {
        return songTrie.searchPrefix(toLowerStr(prefix));
    }

    void relateArtists(const string& artist1, const string& artist2) {
        artistGraph.addEdge(artist1, artist2);
    }

    void printArtistRelations(const string& startArtist) {
        cout << "\n=== ARTISTAS RELACIONADOS CON: " << startArtist << " ===\n";
        if (!artistGraph.hasNode(startArtist)) {
            cout << " No hay relaciones registradas.\n";
        }
        else {
            artistGraph.dfs(startArtist, [](const string& artist) {
                cout << " -> " << artist << "\n";
                });
        }
        cout << "=========================\n";
    }

    void printArtists() const {
        cout << "\n=== ARTISTAS ===\n";
        for (const auto& artist : artists) {
            cout << " " << artist.getName() << " (" << artist.getSongCount() << " canciones)\n";
        }
        if (artists.empty()) cout << " No hay artistas registrados.\n";
        cout << "================\n";
    }

    void printAlbums() const {
        cout << "\n=== ALBUMES/GENEROS ===\n";
        for (const auto& album : albums) {
            cout << " " << album.getTitle() << " - " << album.getArtistName()
                << " (" << album.getTrackCount() << " tracks)\n";
        }
        if (albums.empty()) cout << " No hay albumes.\n";
        cout << "=======================\n";
    }

    void addToPlaybackHistory(const Song& song) {
        playbackHistory.insertAtTail(song);
    }

    void printPlaybackHistory() const {
        cout << "\n=== HISTORIAL CIRCULAR ===\n";
        if (playbackHistory.isEmpty()) {
            cout << " Historial vacio.\n";
        }
        else {
            Node<Song>* curr = playbackHistory.getHead();
            uint i = 0;
            do {
                cout << " " << i++ << ". " << curr->getValue().getName() << "\n";
                curr = curr->next;
            } while (curr != playbackHistory.getHead() && i < playbackHistory.getSize());
        }
        cout << "==========================\n";
    }

    void addToSessionHistory(const Song& song) {
        if (!sessionHistory.isEmpty()) {
            Song top = sessionHistory.top();
            if (top.getSource() == song.getSource()) return; // evita duplicado consecutivo
        }
        sessionHistory.push(song);
    }
    Stack<Song>* getSessionHistory() { return &sessionHistory; }

    /**
     * @brief Limpia TODA la biblioteca sin destruir el objeto.
     * Usa esto en vez de 'lib = MusicLibrary()' para evitar el crash.
     */
    void clear() {
        playlists.clear();
        currentPlaylistIndex = -1;
        allSongs.clear();
        songByNameTree.clear();
        songTrie.clear();
        artistGraph.clear();
        artists.clear();
        albums.clear();
        playbackHistory.clear();
        sessionHistory.clear();   
    }

    void printPlaylists() const {
        cout << "\n=== TUS PLAYLISTS ===\n";
        for (int i = 0; i < (int)playlists.size(); i++) {
            cout << (i == currentPlaylistIndex ? " > " : "   ")
                << i << ". " << playlists[i].getName()
                << " (" << playlists[i].getSize() << " canciones)\n";
        }
        if (playlists.empty()) cout << " No tienes playlists.\n";
        cout << "=====================\n";
    }

    void printLibrary() const {
        cout << "\n=== BIBLIOTECA GLOBAL ===\n";
        Node<Song>* curr = allSongs.getHead();
        uint i = 0;
        while (curr != nullptr) {
            cout << " " << i++ << ". " << curr->getValue().getName()
                << " - " << curr->getValue().getAuthor() << "\n";
            curr = curr->next;
        }
        if (allSongs.isEmpty()) cout << " Biblioteca vacia.\n";
        cout << "=========================\n";
    }
};
