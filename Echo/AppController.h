#pragma once
#include "Interface.h"
#include "MusicLibrary.h"
#include "FileManager.h"
#include "GestorAudio.h"
#include <conio.h>
#include <windows.h>
#include <vector>
#include <string>

using namespace std;

enum class Tab {
    LIBRARY = 1,
    ARTISTS = 2,
    PLAYLISTS = 3,
    QUEUE = 4,
    SEARCH = 5
};

class AppController
{
private:
    Interface ui;
    MusicLibrary musicLib;
    Tab currentTab;
    int currentTabIndex;
    GestorAudio audio;

    int librarySelectedIndex;
    int libraryTopIndex;

    int playlistsSelectedIndex;
    int playlistsTopIndex;

    int artistsSelectedIndex;
    int artistsTopIndex;

    int queueSelectedIndex;
    int queueTopIndex;

    // Estado de busqueda (copias seguras, no punteros)
    vector<Song> searchResults;
    string searchQuery;
    int searchSelectedIndex;
    int searchTopIndex;

    void render(string type);
    void renderRefresh();
    void renderSwap();
    void handleInput();
    void moveDown();
    void moveUp();
    void playSelectedSong();

    // Helpers de busqueda
    void moveDownSearch();
    void moveUpSearch();
    void performSearch();
    void playSelectedSearchSong();

public:
    AppController();
    ~AppController() = default;

    AppController(const AppController&) = delete;
    AppController& operator=(const AppController&) = delete;

    void run();
};
