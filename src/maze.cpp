#include <random>
#include <array>
#include <vector>
#include <stack>
#include <stdexcept>
#include <optional>
#include <algorithm>
#include <set>
#include <iterator>
#include <iostream>
#include "maze.hpp"
#include "node.hpp"

using namespace pathfinder2;

void pathfinder2::generate_maze(NodeMatrix &matrix) {
    // simple random depth first search
    
    std::mt19937 rand{std::random_device{}()};
    std::stack<Point> return_stack{};
    std::vector<Point> dir_opts{};
    std::set<Point> visited_nodes{};
    return_stack.push({0, 0});

    while (!return_stack.empty()) {
        auto cur_node = return_stack.top();
        visited_nodes.insert(cur_node);

        std::vector<Point> wall_nodes_offsets {
            Point{-1, -1}, Point{-1, 0}, Point{-1, 1}, Point{0, 1},
            Point{0, -1}, Point{1, -1}, Point{1, 0}, Point{1, 1},
        };

        dir_opts.clear();
        for (std::size_t i = 0; i < wall_nodes_offsets.size(); i++) {
            auto cur_wall_node = cur_node + wall_nodes_offsets[i];
            if (cur_wall_node.first < 0 ||
                cur_wall_node.second < 0 ||
                cur_wall_node.first >= static_cast<int>(matrix.size()) ||
                cur_wall_node.second >= static_cast<int>(matrix[0].size()))
            {
                wall_nodes_offsets.erase(std::begin(wall_nodes_offsets) + i--);
                continue;
            }

            auto potential_node = cur_wall_node + wall_nodes_offsets[i];
            bool is_in = visited_nodes.find(potential_node) != std::end(visited_nodes);
            bool is_diagonal = wall_nodes_offsets[i].first != 0 && wall_nodes_offsets[i].second != 0;

            if (!is_diagonal && !is_in)
                //if (matrix[cur_wall_node.first][cur_wall_node.second] != Node::Obstical)
                    dir_opts.push_back(wall_nodes_offsets[i]);

            if (is_diagonal || !is_in)
                matrix[cur_wall_node.first][cur_wall_node.second] = Node::Obstical;
        }

        if (dir_opts.empty()) {
            return_stack.pop();
        }
        else {
            auto dir = dir_opts[rand() % dir_opts.size()];
            auto wall_node = cur_node + dir;
            return_stack.push(wall_node + dir);
            matrix[wall_node.first][wall_node.second] = Node::Walkable;
        }
    }
}
