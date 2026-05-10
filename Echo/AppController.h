#pragma once
#include "Interface.h"
#include "MusicLibrary.h"
#include "FileManager.h"
#include "GestorAudio.h"
#include <conio.h>
#include <windows.h>

enum class Tab {
    LIBRARY,
    ARTISTS,
    PLAYLISTS,
    QUEUE,
    SEARCH
};

class AppController
{
private:
    Interface ui;
    MusicLibrary musicLib;
    Tab currentTab;
    GestorAudio audio;

    int librarySelectedIndex;
    int libraryTopIndex;

    int playlistsSelectedIndex;
    int playlistsTopIndex;

    int artistsSelectedIndex;
    int artistsTopIndex;
    
    void render();
    void handleInput();
    void moveDown();
    void moveUp();

public:
    AppController();
    ~AppController() = default;

    AppController(const AppController&) = delete;
    AppController& operator=(const AppController&) = delete;

    void run();
    void playSelectedSong();
};
