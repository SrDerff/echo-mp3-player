#pragma once
#include "Interface.h"
#include "MusicLibrary.h"
#include "FileManager.h"
#include "GestorAudio.h"
#include <conio.h>
#include <windows.h>

enum class Tab {
    LIBRARY=1,
    ARTISTS=2,
    PLAYLISTS=3,
    QUEUE=4,
    SEARCH=5
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

    bool insidePlaylist;
    int openedPlaylistIndex;

    int playlistSongsSelectedIndex;
    int playlistSongsTopIndex;

    int artistsSelectedIndex;
    int artistsTopIndex;

    int queueSelectedIndex;
    int queueTopIndex;
    
    void render(string type);
	void renderRefresh();
	void renderSwap();
    void handleInput();
    void moveDown();
    void moveUp();
    void playSelectedSong();
    void playSelectedPlaylistSong();
    void addSelectedSongToPlaylist();
    void removeSelectedSongFromPlaylist();
    string captureTextInput(const string& prompt);

public:
    AppController();
    ~AppController();

    AppController(const AppController&) = delete;
    AppController& operator=(const AppController&) = delete;

    void run();
};
