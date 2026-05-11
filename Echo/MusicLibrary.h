#pragma once
#include "Playlist.h"
#include "SearchFilter.h"
#include "BinarySearchTree.h"
#include "Tree.h"
#include "Graph.h"
#include "CircularDoublyLinkedList.h"
#include "SortEngine.h"
#include "Artist.h"
#include "Album.h"
#include <vector>
//#include <algorithm>
//#include <cctype>

using namespace std;
using uint = unsigned int;

class MusicLibrary {
private:
    int currentPlaylistIndex;
public: vector<Playlist> playlists;
private:
    DoublyLinkedList<Song> allSongs;
    BinarySearchTree<Song*> songByNameTree;   
    Trie<Song*> songTrie;
    Graph<string> artistGraph;
    vector<Artist> artists;
    vector<Album> albums;
    CircularDoublyLinkedList<Song> playbackHistory;

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
        playlists.reserve(100);
    }

    
    void addPlaylist(const string& name) {
        string newName = name;
        playlists.emplace_back(newName);
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

    vector<Song*> searchInLibrary(const string& query) {
        return SearchFilter<Song>::searchByString(
            allSongs, [](const Song& s) { return s.getName(); }, query);
    }

    vector<Song*> searchByAuthor(const string& query) {
        return SearchFilter<Song>::searchByString(
            allSongs, [](const Song& s) { return s.getAuthor(); }, query);
    }

    vector<Song*> searchByGenre(const string& query) {
        return SearchFilter<Song>::searchByString(
            allSongs, [](const Song& s) { return s.getGenre(); }, query);
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

    vector<Song> getAllSongsVector() {
        vector<Song> songs;
        Node<Song>* curr = allSongs.getHead();

        while (curr != nullptr) {
            songs.push_back(curr->getValue());
            curr = curr->next;
        }

        return songs;
    }

    void createDailyPlaylist() {
        if (playlists.size() >= 5) {
            cout << "Ya tienes 5 playlists.\n";
            return;
        }

        vector<Song> all = getAllSongsVector();
        if (all.empty()) {
            cout << "No hay canciones en la biblioteca.\n";
            return;
        }

        addPlaylist("Daily Mix");
        Playlist* p = getPlaylist(getPlaylistCount() - 1);

        if (!p) return;

        srand(time(nullptr));

        for (int i = static_cast<int>(all.size()) - 1; i > 0; --i) {
            int j = rand() % (i + 1);
            swap(all[i], all[j]);
        }

        size_t limit = min(size_t(10), all.size());

        for (size_t i = 0; i < limit; ++i) {
            p->addSong(all[i]);
        }

        cout << "Daily Mix creada con " << limit << " canciones.\n";
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
    }

};