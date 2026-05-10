#include "AppController.h"

namespace {
    constexpr int kVisibleLibraryRows = 17;
}

static string toLowerStr(string str) {
    for (char& c : str) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return str;
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
    queueTopIndex(0),
    searchSelectedIndex(0),
    searchTopIndex(0)
{
    FileManager::loadSongs(musicLib);
    FileManager::loadPlaylists(musicLib);
    performSearch();
    ui.displayMenu(musicLib, librarySelectedIndex, libraryTopIndex, false);
}

void AppController::renderRefresh() {
    switch (currentTab) {
    case Tab::LIBRARY:
        ui.refreshLibraryRows(musicLib, librarySelectedIndex, libraryTopIndex);
        break;
    case Tab::SEARCH:
        ui.refreshSearchRows(searchResults, searchSelectedIndex, searchTopIndex);
        break;
    case Tab::QUEUE:
        ui.refreshQueueRows(*musicLib.getSessionHistory(), queueSelectedIndex, queueTopIndex);
        break;
    default:
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
        ui.displaySearchWithResults(searchResults, searchSelectedIndex, searchTopIndex, searchQuery);
        break;
    }
}

void AppController::render(string type) {
    if (type == "refresh") {
        renderRefresh();
    }
    else if (type == "swap") {
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

void AppController::moveDownSearch() {
    if (searchResults.empty()) return;
    if (searchSelectedIndex < static_cast<int>(searchResults.size()) - 1) {
        searchSelectedIndex++;
        if (searchSelectedIndex >= searchTopIndex + kVisibleLibraryRows) {
            searchTopIndex++;
        }
    }
}

void AppController::moveUpSearch() {
    if (searchResults.empty()) return;
    if (searchSelectedIndex > 0) {
        searchSelectedIndex--;
        if (searchSelectedIndex < searchTopIndex) {
            searchTopIndex--;
        }
    }
}

void AppController::performSearch() {
    searchResults.clear();
    string lowerQuery = toLowerStr(searchQuery);

    Node<Song>* curr = musicLib.getAllSongs()->getHead();
    while (curr != nullptr) {
        Song song = curr->getValue(); // copia segura

        bool match = false;
        if (searchQuery.empty()) {
            match = true;
        }
        else {
            string lowerName = toLowerStr(song.getName());
            string lowerArtist = toLowerStr(song.getAuthor());
            if (lowerName.find(lowerQuery) != string::npos ||
                lowerArtist.find(lowerQuery) != string::npos) {
                match = true;
            }
        }

        if (match) {
            searchResults.push_back(song); // copia al vector
        }
        curr = curr->next;
    }

    searchSelectedIndex = 0;
    searchTopIndex = 0;
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
    musicLib.addToSessionHistory(selectedSong);
    ui.refreshHud(musicLib, librarySelectedIndex, libraryTopIndex);
}

void AppController::playSelectedSearchSong() {
    if (currentTab != Tab::SEARCH || searchResults.empty()) return;
    Song& selectedSong = searchResults[searchSelectedIndex];
    if (audio.getActual() != selectedSong.getSource()) {
        audio.cerrar();
        if (audio.cargar(selectedSong.getSource())) {
            audio.reproducir();
        }
    }
    else {
        audio.reproducir();
    }
    musicLib.addToSessionHistory(selectedSong);  
    ui.refreshHudSong(selectedSong.getName(), selectedSong.getAuthor());
}

void AppController::moveDownQueue() {
    Stack<Song>* history = musicLib.getSessionHistory();
    int totalSongs = static_cast<int>(history->size());
    if (totalSongs == 0) return;
    if (queueSelectedIndex < totalSongs - 1) {
        queueSelectedIndex++;
        if (queueSelectedIndex >= queueTopIndex + kVisibleLibraryRows) {
            queueTopIndex++;
        }
    }
}

void AppController::moveUpQueue() {
    if (queueSelectedIndex > 0) {
        queueSelectedIndex--;
        if (queueSelectedIndex < queueTopIndex) {
            queueTopIndex--;
        }
    }
}

void AppController::playSelectedQueueSong() {
    if (currentTab != Tab::QUEUE) return;
    Stack<Song>* history = musicLib.getSessionHistory();
    if (history->isEmpty()) return;
    if (queueSelectedIndex >= static_cast<int>(history->size())) return;

    Song selectedSong = history->getAt(queueSelectedIndex);
    if (audio.getActual() != selectedSong.getSource()) {
        audio.cerrar();
        if (audio.cargar(selectedSong.getSource())) {
            audio.reproducir();
        }
    }
    else {
        audio.reproducir();
    }
    musicLib.addToSessionHistory(selectedSong);
    ui.refreshHudSong(selectedSong.getName(), selectedSong.getAuthor());
}

void AppController::handleInput() {
    if (!_kbhit()) return;

    int key = _getch();

    if (key == 13) { // Enter
        if (currentTab == Tab::SEARCH && !searchResults.empty()) {
            playSelectedSearchSong();
        }
        else if (currentTab == Tab::QUEUE) {   
            playSelectedQueueSong();           
        }
        else {
            playSelectedSong();
        }
        return;
    }
    else if (key == 32) { // Space
        if (!audio.estaPausado()) audio.pausar();
        else audio.reproducir();
        return;
    }

    if (currentTab == Tab::SEARCH) {
        if (key == 8) { // Backspace
            if (!searchQuery.empty()) {
                searchQuery.pop_back();
                performSearch();
                render("swap");
            }
            return;
        }
        if (key >= 32 && key <= 126) {
            searchQuery += static_cast<char>(key);
            performSearch();
            render("swap");
            return;
        }
    }

    if (key != 224) return;
    key = _getch();

    switch (key) {
    case 75: // Left
        if (currentTab == Tab::LIBRARY) return;
        if (currentTab == Tab::PLAYLISTS) {
            currentTab = Tab::LIBRARY;
            render("swap");
        }
        else if (currentTab == Tab::ARTISTS) {
            currentTab = Tab::PLAYLISTS;
            render("swap");
        }
        else if (currentTab == Tab::QUEUE) {
            currentTab = Tab::ARTISTS;
            render("swap");
        }
        else if (currentTab == Tab::SEARCH) {
            currentTab = Tab::QUEUE;
            render("swap");
        }
        break;
    case 77: // Right
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
    case 80: // Down
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
        else if (currentTab == Tab::SEARCH) {
            int previousSelectedIndex = searchSelectedIndex;
            int previousTopIndex = searchTopIndex;
            moveDownSearch();
            if (searchTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (searchSelectedIndex != previousSelectedIndex) {
                ui.refreshSearchSelection(searchResults, previousSelectedIndex, searchSelectedIndex, searchTopIndex);
            }
        }
        else if (currentTab == Tab::QUEUE) {   
            int previousSelectedIndex = queueSelectedIndex;
            int previousTopIndex = queueTopIndex;
            moveDownQueue();
            if (queueTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (queueSelectedIndex != previousSelectedIndex) {
                ui.refreshQueueSelection(*musicLib.getSessionHistory(), previousSelectedIndex, queueSelectedIndex, queueTopIndex);
            }
        }
        break;
    case 72: // Up
        if (currentTab == Tab::LIBRARY) {
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
        else if (currentTab == Tab::SEARCH) {
            int previousSelectedIndex = searchSelectedIndex;
            int previousTopIndex = searchTopIndex;
            moveUpSearch();
            if (searchTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (searchSelectedIndex != previousSelectedIndex) {
                ui.refreshSearchSelection(searchResults, previousSelectedIndex, searchSelectedIndex, searchTopIndex);
            }
            else if (currentTab == Tab::QUEUE) {   
                int previousSelectedIndex = queueSelectedIndex;
                int previousTopIndex = queueTopIndex;
                moveUpQueue();
                if (queueTopIndex != previousTopIndex) {
                    render("refresh");
                }
                else if (queueSelectedIndex != previousSelectedIndex) {
                    ui.refreshQueueSelection(*musicLib.getSessionHistory(), previousSelectedIndex, queueSelectedIndex, queueTopIndex);
                }
            }
        }
        break;
    default:
        break;
    }
}

void AppController::run() {
    while (true) {
        handleInput();
        ui.drawSpectrum(130, 50, !audio.estaPausado());
    }
}
