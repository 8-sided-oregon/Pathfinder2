#pragma once

#include <cmath>
#include <vector>
#include <array>
#include <cstdint>
#include <SDL2/SDL.h>
#include "graphics.hpp"

namespace pathfinder2 {
    enum class Node {
        Walkable,
        Obstical,
        Start,
        End,
    };

    Node &operator++(Node &rhs);
    Node &operator--(Node &rhs);

    constexpr const int node_grid_width = 15;
    constexpr const int node_grid_height = 15;
    
    // vector of arrays instead of vector of vectors so that nodes are in one contigous
    // chunk of memory
    using NodeMatrix = std::vector<std::array<Node, node_grid_height>>;
    using Point = std::pair<int, int>;
    using Color = SDL_Color;

    float dist(Point a, Point b);

    Point operator+(Point a, Point b);
}
