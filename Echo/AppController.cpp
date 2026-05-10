#include "AppController.h"

namespace {
    constexpr int kVisibleLibraryRows = 17;
}

AppController::AppController()
    : ui(100, 60),
      currentTab(Tab::LIBRARY),
      librarySelectedIndex(0),
      libraryTopIndex(0),
      playlistsSelectedIndex(0),
      playlistsTopIndex(0),
      artistsSelectedIndex(0),
      artistsTopIndex(0)
{
    FileManager::loadSongs(musicLib);
    FileManager::loadPlaylists(musicLib);
    ui.displayMenu(musicLib, librarySelectedIndex, libraryTopIndex);
}

void AppController::render() {
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
}

void AppController::handleInput() {
    if (!_kbhit()) return;

    int key = _getch();

    if (key == 13) { // Enter
        playSelectedSong();
        return;
    }
    else if (key== 32) { // Escape
        if (!audio.estaPausado()) audio.pausar();
        else audio.reproducir();
        return;
	}

	//para las flechas, el primer _getch() devuelve 224, y el segundo devuelve el codigo de la flecha
    if (key != 224) return;

    key = _getch();

    switch (key) {
    case 80:
        if (currentTab == Tab::LIBRARY) {
            int previousSelectedIndex = librarySelectedIndex;
            int previousTopIndex = libraryTopIndex;
            moveDown();

            if (libraryTopIndex != previousTopIndex) {
                render();
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
                render();
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

void AppController::run() {
    while (true) {
		handleInput(); //// Maneja la entrada del usuario en cada iteracion del loop principal.
    }
}
