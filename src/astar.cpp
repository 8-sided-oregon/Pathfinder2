#include <stdexcept>
#include <vector>
#include <algorithm>
#include <array>
#include <iostream>
#include <format>
#include "pathing.hpp"
#include "node.hpp"

using namespace pathfinder2;

int calculate_h_cost(std::vector<Point> search_nodes, std::vector<int> parent_nodes, int ind) {
    int accum_dist = 0;
    for (; parent_nodes[ind] != -1; ind = parent_nodes[ind]) {
        auto current_node = search_nodes[ind];
        auto parent_node = search_nodes[parent_nodes[ind]];
        accum_dist += std::floor(10 * dist(current_node, parent_node));
    }
    return accum_dist;
}

std::vector<PathPoint> AStar::find_path(const NodeMatrix &matrix) {
    // linear search to find start and end nodes

    Point start_point{-1, -1}, end_point{-1, -1};
    int x = 0;
    for (const auto &col : matrix) {
        int y = 0;
        for (const auto &node : col) {
            if (node == Node::Start)
                start_point = {x, y};
            if (node == Node::End)
                end_point = {x, y};
            y++;
        }
        x++;
    }

    if (start_point.first < 0 || end_point.first < 0)
        throw std::invalid_argument("No start and/or end point");

    // do algorithm

    std::vector<Point> search_nodes{};
    std::vector<int> parent_nodes{};

    search_nodes.push_back(start_point);
    parent_nodes.push_back(-1);

    for (;;) {
        Point lowest_f_cost_point{};
        int lowest_f_cost_cost = -1, lowest_f_cost_parent = -1;

        // search for node with lowest f cost
        int search_nodes_len = static_cast<int>(search_nodes.size());
        for (int i = 0; i < search_nodes_len; i++) {
            auto current_node = search_nodes[i];

            std::array<Point, 8> contenders = {
                current_node + Point{0, 1},
                current_node + Point{1, 0},
                current_node + Point{0, -1},
                current_node + Point{-1, 0},
                current_node + Point{-1, -1},
                current_node + Point{1, 1},
                current_node + Point{1, -1},
                current_node + Point{-1, 1},
            };

            for (auto contender : contenders) {
                // dont search through already searched nodes 
                if (std::find(std::begin(search_nodes), std::end(search_nodes), contender) != std::end(search_nodes))
                    continue;

                // bounds check and dont search if it's an obstical obv
                if (contender.first < 0 || 
                        contender.second < 0 || 
                        contender.first >= static_cast<int>(matrix.size()) || 
                        contender.second >= static_cast<int>(matrix[0].size()) ||
                        matrix[contender.first][contender.second] == Node::Obstical)
                    continue;

                int h_cost_lhs = std::floor(10 * dist(contender, search_nodes[i]));
                int h_cost_rhs = calculate_h_cost(search_nodes, parent_nodes, i);
                int g_cost = std::floor(10 * dist(contender, end_point));
                int f_cost = h_cost_lhs + h_cost_rhs + g_cost;
                
                if (f_cost < lowest_f_cost_cost || lowest_f_cost_cost == -1) {
                    lowest_f_cost_point = contender;
                    lowest_f_cost_parent = i;
                    lowest_f_cost_cost = f_cost;
                }
            }
        }
        
        if (lowest_f_cost_cost < 0)
            return {}; // no possible way to endpoint

        search_nodes.push_back(lowest_f_cost_point);
        parent_nodes.push_back(lowest_f_cost_parent);

        // updates parents of surrounding nodes if the new node gets them a lower hcost
        std::array<Point, 8> nodes_to_update = {
            lowest_f_cost_point + Point{0, 1},
            lowest_f_cost_point + Point{1, 0},
            lowest_f_cost_point + Point{0, -1},
            lowest_f_cost_point + Point{-1, 0},
            lowest_f_cost_point + Point{-1, -1},
            lowest_f_cost_point + Point{1, 1},
            lowest_f_cost_point + Point{1, -1},
            lowest_f_cost_point + Point{-1, 1},
        };

        for (auto cur_node : nodes_to_update) {
            auto found = std::find(std::begin(search_nodes), std::end(search_nodes), cur_node);
            if (found == std::end(search_nodes))
                continue;
            int ind = found - std::begin(search_nodes);

            int h_cost_lhs_post  = std::floor(10 * dist(*found, lowest_f_cost_point));
            int h_cost_rhs_post  = calculate_h_cost(search_nodes, parent_nodes, search_nodes.size() - 1);
            int h_cost_rhs_pre  = calculate_h_cost(search_nodes, parent_nodes, ind);

            if (h_cost_lhs_post + h_cost_rhs_post < h_cost_rhs_pre)
                parent_nodes[ind] = search_nodes.size() - 1;
        }

        // checks if we're done
        if (lowest_f_cost_point == end_point)
            break;
    }

    // paint searched and optimal nodes

    std::vector<PathPoint> ret{};
    
    for (int i = 0; i < static_cast<int>(search_nodes.size()); i++) {
        int h_cost = calculate_h_cost(search_nodes, parent_nodes, i);
        int g_cost = std::floor(10 * dist(search_nodes[i], end_point));
        auto msg = std::format("h_cost: {:3} g_cost: {:3} f_cost: {:3}", h_cost, g_cost, h_cost + g_cost);
        ret.push_back({search_nodes[i], false, msg});
    }

    for (int i = search_nodes.size() - 1; parent_nodes[i] != -1; i = parent_nodes[i])
        ret[i].is_optimal = true;

    return ret;
}
