#pragma once
#include "MusicLibrary.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
using namespace std;

class Interface
{
private:
    int height;
    int width;

    void writeAt(int x, int y, const string& text, int fr, int fg, int fb);
    void writeAt(int x, int y, const string& text, int fr, int fg, int fb, int br, int bg, int bb);
    void hLine(int x, int y, int len, char ch, int fr, int fg, int fb);
    void vLine(int x, int y, int len, char ch, int fr, int fg, int fb);
    void fillRect(int x, int y, int w, int h, char ch, int fr, int fg, int fb);
    void fillRect(int x, int y, int w, int h, char ch, int fr, int fg, int fb, int br, int bg, int bb);
    void drawBox(int x, int y, int w, int h, int fr, int fg, int fb);
    void drawProgressBar(int x, int y, int w);
    void drawVolumeBar(int x, int y, int level);
    void drawTabs(int x, int y);
    void drawTableHeader(int x, int y);
    void drawLibraryRows(int x, int y, MusicLibrary& library, int selectedIndex, int topIndex);
    void drawQueueRows(int x, int y);
    void drawRightPanelPlaceholder(int x, int y, int w, int h);

    void drawBottomSeekbar(int x, int y, int w);
    void drawAlbumArt(int x, int y, const int art[25][25]);
    void getAlbumArtColor(int code, int& r, int& g, int& b);

public:
    Interface() {}
    Interface(int width, int height) : width(width), height(height) {}
    ~Interface() {}

    void displayHud(MusicLibrary& library, int selectedIndex, int topIndex);
    void displayTab();
    void displayQueue();
    void displayPlaylists();
    void displayLibrary(MusicLibrary& library, int selectedIndex, int topIndex);
    void displayArtists();
    void displaySearch();
    void displayMenu(MusicLibrary& library, int selectedIndex, int topIndex, bool playing);
    void displayBackground();
    void drawSpectrum(int x, int y, bool playing);

    //refresh
    void refreshLibrarySelection(MusicLibrary& library, int previousSelectedIndex, int selectedIndex, int topIndex);
	void refreshHud(MusicLibrary& library, int selectedIndex, int topIndex);
    void refreshLibraryRows(MusicLibrary& library, int selectedIndex, int topIndex);
};
