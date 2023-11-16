#pragma once

#include <cmath>
#include <vector>
#include <cstdint>
#include <SDL2/SDL.h>

namespace pathfinder2 {
    enum class Node {
        Walkable,
        Obstical,
        Start,
        End,
    };

    Node &operator++(Node &rhs);
    Node &operator--(Node &rhs);

    using NodeMatrix = std::vector<std::vector<Node>>;
    using Point = std::pair<int, int>;
    using Color = SDL_Color;

    float dist(Point a, Point b);

    Point operator+(Point a, Point b);
}
