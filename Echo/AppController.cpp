#include "AppController.h"

namespace {
    constexpr int kVisibleLibraryRows = 17;
}

AppController::AppController()
    : ui(100, 60),
      currentTab(Tab::LIBRARY),
	  currentTabIndex(1),
      librarySelectedIndex(0),
      libraryTopIndex(0),
      playlistsSelectedIndex(0),
      playlistsTopIndex(0),
      artistsSelectedIndex(0),
      artistsTopIndex(0),
	  queueSelectedIndex(0),
	  queueTopIndex(0)
{
    FileManager::loadSongs(musicLib);
    FileManager::loadPlaylists(musicLib);
    musicLib.createDailyPlaylist();
    ui.displayMenu(musicLib, librarySelectedIndex, libraryTopIndex, false);
}

AppController::~AppController() {
	FileManager::saveSongs(musicLib);
	FileManager::savePlaylists(musicLib);
}

void AppController::renderRefresh() {
    switch (currentTab) {
    case Tab::LIBRARY:
        ui.refreshLibraryRows(musicLib, librarySelectedIndex, libraryTopIndex);
        break;
    case Tab::PLAYLISTS:
    case Tab::ARTISTS:
    case Tab::QUEUE:
    case Tab::SEARCH:
    default:
        ui.refreshLibraryRows(musicLib, librarySelectedIndex, libraryTopIndex);
        break;
    }
}

void AppController::renderSwap() {
    switch (currentTab) {
    case Tab::LIBRARY:
        ui.displayLibrary(musicLib, librarySelectedIndex, libraryTopIndex);
        break;
    case Tab::PLAYLISTS:
        ui.displayPlaylists(musicLib, playlistsSelectedIndex, playlistsTopIndex);
        break;
    case Tab::ARTISTS:
        ui.displayArtists(musicLib, artistsSelectedIndex, artistsTopIndex);
        break;
    case Tab::QUEUE:
        ui.displayQueue(musicLib, queueSelectedIndex, queueTopIndex);
		break;
    case Tab::SEARCH:
        ui.displaySearch(musicLib, playlistsSelectedIndex, playlistsTopIndex); //parametros para corregir
		break;  
    }
}

void AppController::render(string type) {
    if(type=="refresh") {
        renderRefresh();
    }
    else if(type=="swap") {
        renderSwap();
    }
}

void AppController::moveDown() {
    if (currentTab != Tab::LIBRARY) return;

    const int totalSongs = static_cast<int>(musicLib.getAllSongs()->getSize());
    if (totalSongs == 0) return;

    if (librarySelectedIndex < totalSongs - 1) {
        librarySelectedIndex++;
        if (librarySelectedIndex >= libraryTopIndex + kVisibleLibraryRows) {
            libraryTopIndex++;
        }
    }
}

void AppController::moveUp() {
    if (currentTab != Tab::LIBRARY) return;

    if (librarySelectedIndex > 0) {
        librarySelectedIndex--;
        if (librarySelectedIndex < libraryTopIndex) {
            libraryTopIndex--;
        }
    }
}

void AppController::handleInput() {
    if (!_kbhit()) return;

    int key = _getch();
    if(key== 27) { // Escape
        FileManager::savePlaylists(musicLib);
        exit(0);
	}
    if (key == 13) { // Enter
        playSelectedSong();
        return;
    }
    else if (key== 32) { // Espacio
        if (!audio.estaPausado()) audio.pausar();
        else audio.reproducir();
        return;
	}

    if (key != 224) return;
    key = _getch();

    switch (key) {
    case 75:
        if (currentTab == Tab::LIBRARY) return;
        if (currentTab == Tab::PLAYLISTS) {
			currentTab = Tab::LIBRARY;
            render("swap");
        }
        else if(currentTab == Tab::ARTISTS) {
			currentTab = Tab::PLAYLISTS;
            render("swap");
        }
        else if(currentTab == Tab::QUEUE) {
			currentTab = Tab::ARTISTS;
            render("swap");
        }
        else if(currentTab == Tab::SEARCH) {
			currentTab = Tab::QUEUE;
            render("swap");
        }
		break;
    case 77:
        if (currentTab == Tab::SEARCH) return;
        if (currentTab == Tab::LIBRARY) {
			currentTab = Tab::PLAYLISTS;
            render("swap");
        }
        else if (currentTab == Tab::PLAYLISTS) {
			currentTab = Tab::ARTISTS;
            render("swap");
        }
        else if (currentTab == Tab::ARTISTS) {
            currentTab = Tab::QUEUE;
            render("swap");
        }
        else if (currentTab == Tab::QUEUE) {
			currentTab = Tab::SEARCH;
            render("swap");
        }
        break;
    case 80:
        if (currentTab == Tab::LIBRARY) {
            int previousSelectedIndex = librarySelectedIndex;
            int previousTopIndex = libraryTopIndex;
            moveDown();

            if (libraryTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (librarySelectedIndex != previousSelectedIndex) {
                ui.refreshLibrarySelection(musicLib, previousSelectedIndex, librarySelectedIndex, libraryTopIndex);
            }
        }
        break;
    case 72:
        if(currentTab== Tab::LIBRARY) {
            int previousSelectedIndex = librarySelectedIndex;
            int previousTopIndex = libraryTopIndex;
            moveUp();
            if (libraryTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (librarySelectedIndex != previousSelectedIndex) {
                ui.refreshLibrarySelection(musicLib, previousSelectedIndex, librarySelectedIndex, libraryTopIndex);
            }
		}
        break;
    default:
        break;
    }
}

void AppController::playSelectedSong() {
    if (currentTab != Tab::LIBRARY) return;

    Song selectedSong = musicLib.getAllSongs()->getAt(librarySelectedIndex);

    if (audio.getActual() != selectedSong.getSource()) {
        audio.cerrar();
        if (audio.cargar(selectedSong.getSource())) {
            audio.reproducir();
        }
    }
    else {
        audio.reproducir();
    }
    ui.refreshHud(musicLib, librarySelectedIndex, libraryTopIndex);
}

void AppController::run() {
    while (true) {
		handleInput(); // Maneja la entrada del usuario en cada iteracion del loop principal.
		ui.drawSpectrum(130, 50, !audio.estaPausado()); // Dibuja el espectro en la parte inferior de la pantalla.
    }
}
