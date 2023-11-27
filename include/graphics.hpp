#pragma once

#include <SDL_pixels.h>
#include <string>
#include <SDL2/SDL.h>

#define BASE_ASSET_PATH "../assets/"

namespace pathfinder2 {
    namespace ui {
        constexpr const int window_width = 900;
        constexpr const int window_height = 1000;

        constexpr const int node_height_px = 60;
        constexpr const int node_width_px = 60;

        constexpr const char *walkable_texture_path = BASE_ASSET_PATH "00walkable.bmp";
        constexpr const char *obstical_texture_path = BASE_ASSET_PATH "01obstical.bmp";
        constexpr const char *start_texture_path = BASE_ASSET_PATH "02start.bmp";
        constexpr const char *end_texture_path = BASE_ASSET_PATH "03end.bmp";
        constexpr const char *suboptimal_texture_path = BASE_ASSET_PATH "04suboptimal.bmp";
        constexpr const char *optimal_texture_path  = BASE_ASSET_PATH "05optimal.bmp";
        
        constexpr const char *font_asset_path = BASE_ASSET_PATH "PixelOperatorMono.ttf";
        constexpr const int font_asset_pt = 20;
        constexpr const SDL_Color font_color = SDL_Color { 255, 255, 255, 255 };

        constexpr const int frame_counter_pt = 20;

        int run();
    }
}
