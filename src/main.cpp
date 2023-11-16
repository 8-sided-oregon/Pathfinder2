#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "node.hpp"
#include "graphics.hpp"

int main() {
    int rc = 0;

    try {
        rc = pathfinder2::ui::run();
    }
    catch (std::runtime_error &e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        rc = 1;
    }

    // Teardown, safe to do even if initialization failed

    TTF_Quit();
    SDL_Quit();

    return rc;
}
