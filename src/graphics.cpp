#include <SDL_mouse.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <algorithm>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "node.hpp"
#include "graphics.hpp"
#include "pathing.hpp"
#include "maze.hpp"

using namespace pathfinder2;
using namespace pathfinder2::ui;

void print_sdl_err(const char *msg) {
    std::cerr << msg << "! SDL_Error: " << SDL_GetError() << "\n";
}

void texture_deleter(SDL_Texture *texture) { SDL_DestroyTexture(texture); }

SDL_Texture *load_texture(SDL_Renderer *renderer, const char *filename) {
    SDL_Surface *surf = SDL_LoadBMP(filename);

    if (surf == nullptr) {
        auto msg = std::string{"failed loading texture "} + filename;
        print_sdl_err(msg.c_str());
        throw std::runtime_error("failed loading texture");
    }

    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    if (text == nullptr) {
        auto msg = std::string{"failed creating surface from texture "} + filename;
        print_sdl_err(msg.c_str());
        throw std::runtime_error("failed to create surface from texture");
    }

    return text;
}

struct GameTextures {
    using text_ptr = std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)>;

    text_ptr walkable, obstical, start, end, suboptimal, optimal;

    GameTextures(SDL_Renderer *renderer) :
        walkable{load_texture(renderer, walkable_texture_path), texture_deleter},
        obstical{load_texture(renderer, obstical_texture_path), texture_deleter},
        start{load_texture(renderer, start_texture_path), texture_deleter},
        end{load_texture(renderer, end_texture_path), texture_deleter},
        suboptimal{load_texture(renderer, suboptimal_texture_path), texture_deleter},
        optimal{load_texture(renderer, optimal_texture_path), texture_deleter}
    {
        SDL_Point node_text_size;
        SDL_QueryTexture(&*walkable, nullptr, nullptr, &node_text_size.x, &node_text_size.y);
        if (node_text_size.x != node_width_px || node_text_size.y != node_height_px)
            throw std::runtime_error("incorrect texture size loaded");
    }

    SDL_Texture &node2text(Node node) {
        switch (node) {
            case Node::Walkable: return *walkable;
            case Node::Obstical: return *obstical;
            case Node::End: return *end;
            case Node::Start: return *start;
        }
    }
};

void draw_cells(
        const NodeMatrix &nodes, 
        const std::vector<PathPoint> &path_points, 
        SDL_Renderer &renderer, 
        GameTextures &textures) 
{
    SDL_Rect dst{0, 0, node_width_px, node_height_px};
    int outer_len = nodes.size();
    int inner_len = nodes[0].size();
    for (int x = 0; x < outer_len; x++) {
        for (int y = 0; y < inner_len; y++) {
            dst.x = x * node_width_px;
            dst.y = y * node_height_px;
            SDL_RenderCopy(&renderer, &textures.node2text(nodes[x][y]), nullptr, &dst);
        }
    }

    for (const auto &current_point : path_points) {
        if (nodes[current_point.point.first][current_point.point.second] != Node::Walkable)
            continue;

        dst.x = current_point.point.first * node_width_px;
        dst.y = current_point.point.second * node_height_px;
        SDL_Texture *text = nullptr;
        if (current_point.is_optimal)
            text = &*textures.optimal;
        else
            text = &*textures.suboptimal;
        SDL_RenderCopy(&renderer, text, nullptr, &dst);
    }
}

// all of this code is exception safe so dw
void draw_msg(const char *msg, TTF_Font &font, SDL_Renderer &renderer) noexcept {
    // clear the text portion of the window

    SDL_Rect dst{
        0, 
        node_grid_height * node_height_px, 
        window_width, 
        window_height - node_grid_height * node_height_px};
    SDL_SetRenderDrawColor(&renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(&renderer, &dst);
    
    // create and draw the text

    SDL_Surface *text_surf = TTF_RenderText_Solid_Wrapped(&font, msg, font_color, window_width);
    SDL_Texture *text_text = SDL_CreateTextureFromSurface(&renderer, &*text_surf);

    SDL_GetClipRect(text_surf, &dst);
    dst.x = 0;
    dst.y = node_grid_height * node_height_px;

    SDL_RenderCopy(&renderer, text_text, nullptr, &dst);

    //deinit
    SDL_FreeSurface(text_surf);
    SDL_DestroyTexture(text_text);
}

// all of this code is exception safe so dw
void draw_frame_ctr(TTF_Font &font, SDL_Renderer &renderer) noexcept {
    static unsigned long frame_cnt = 0;
    
    // create and draw the text

    auto text = std::to_string(frame_cnt++);
    SDL_Surface *text_surf = TTF_RenderText_Solid_Wrapped(&font, text.c_str(), font_color, window_width);
    SDL_Texture *text_text = SDL_CreateTextureFromSurface(&renderer, &*text_surf);

    SDL_Rect dst{};
    SDL_GetClipRect(text_surf, &dst);
    dst.x = window_width - dst.w;
    dst.y = 0;

    // clear the top right portion of the window

    SDL_SetRenderDrawColor(&renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(&renderer, &dst);

    // draw the text 

    SDL_RenderCopy(&renderer, text_text, nullptr, &dst);

    //deinit
    
    SDL_FreeSurface(text_surf);
    SDL_DestroyTexture(text_text);
}

int pathfinder2::ui::run() {
    // SDL init stuff

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        print_sdl_err("SDL couldn't initialize");
        return -1;
    }

    std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> window{
        SDL_CreateWindow("Pathfinder2", 
                SDL_WINDOWPOS_UNDEFINED, 
                SDL_WINDOWPOS_UNDEFINED, 
                window_width, 
                window_height, 
                SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL),
        [](SDL_Window *window) { SDL_DestroyWindow(window); },
    };

    if (window == nullptr) {
        print_sdl_err("Couldn't create window");
        return -1;
    }

    std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer *)> renderer{
        SDL_CreateRenderer(&*window, -1, SDL_RENDERER_ACCELERATED),
        [](SDL_Renderer *renderer) { SDL_DestroyRenderer(renderer); },
    };

    if (renderer == nullptr) {
        print_sdl_err("Couldn't create renderer");
        return -1;
    }

    GameTextures textures{&*renderer};
    NodeMatrix node_matrix{node_grid_width};
    generate_maze(node_matrix);

    // TTF init stuff

    if (TTF_Init() < 0) {
        print_sdl_err("SDL_ttf couldn't initialize");
        return -1;
    }

    std::unique_ptr<TTF_Font, void (*)(TTF_Font *)> app_font{
        TTF_OpenFont(font_asset_path, font_asset_pt),
        [](TTF_Font *font) { TTF_CloseFont(font); },
    };

    std::unique_ptr<TTF_Font, void (*)(TTF_Font *)> frame_cnt_font{
        TTF_OpenFont(font_asset_path, frame_counter_pt),
        [](TTF_Font *font) { TTF_CloseFont(font); },
    };

    // Main event loop

    SDL_RenderClear(&*renderer);

    AStar pathing_algo{};
    std::vector<PathPoint> pathing_result{};
    int last_mouse_x = -1, last_mouse_y = -1;

    draw_msg("e - generate maze | c - clear board | s - change algorithm | q - quit", *app_font, *renderer);

    for (bool quit_flag = false; !quit_flag;) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            bool recompute_required = false;

            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)) {
                std::cerr << "Quiting...\n";
                quit_flag = true;
            }

            if (event.type == SDL_KEYDOWN) {
                auto pressed = event.key.keysym.sym;
                if (pressed == SDLK_e) {
                    for (auto &col : node_matrix)
                        col.fill(Node::Walkable);
                    generate_maze(node_matrix);
                } 
                else if (pressed == SDLK_c) {
                    for (auto &col : node_matrix)
                        col.fill(Node::Walkable);
                }
                else if (pressed == SDLK_e) {
                    // TODO
                }

                recompute_required = true;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x / node_width_px;
                int y = event.button.y / node_height_px;

                if (event.button.button == SDL_BUTTON_LEFT) {
                    ++node_matrix[x][y];
                    recompute_required = true;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT) {
                    --node_matrix[x][y];
                    recompute_required = true;
                }
            }

            if (recompute_required) {
                // confirm that there's only one start and end node
                int start_cnt = 0, end_cnt = 0;
                for (const auto &col : node_matrix) {
                    for (auto node : col) {
                        if (node == Node::Start)
                            start_cnt++;
                        else if (node == Node::End)
                            end_cnt++;
                    }
                }

                if (start_cnt == 1 && end_cnt == 1) {
                    pathing_result = pathing_algo.find_path(node_matrix);
                    if (pathing_result.size() == 0)
                        draw_msg("There is no way to the end node from the start node", *app_font, *renderer);
                }
                else {
                    pathing_result = {};
                    if (start_cnt != 1)
                        draw_msg("There has to be exactly one start (blue) node", *app_font, *renderer);
                    else if (end_cnt != 1)
                        draw_msg("There has to be exactly one end (red) node", *app_font, *renderer);
                }
            }

            draw_cells(node_matrix, pathing_result, *renderer, textures);
        }

        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);

        // draws text for the cell underneeth the cursor when the cursor position changes
        if (last_mouse_x != mouse_x || last_mouse_y != mouse_y) {
            for (const auto &cur_result : pathing_result) {
                if (mouse_x / node_width_px == cur_result.point.first &&
                    mouse_y / node_height_px == cur_result.point.second &&
                    cur_result.text != std::nullopt) 
                {
                    draw_msg(cur_result.text->c_str(), *app_font, *renderer);
                    break;
                }
            }

            last_mouse_x = mouse_x;
            last_mouse_y = mouse_y;
        }

        draw_frame_ctr(*frame_cnt_font, *renderer);
        
        SDL_RenderPresent(&*renderer);

        // ~60fps
        SDL_Delay(17);
    }

    // Teardown of SDL and SDL_ttf is done in main()

    return 0;
}
