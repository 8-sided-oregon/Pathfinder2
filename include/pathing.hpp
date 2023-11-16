#pragma once

#include "node.hpp"
#include <string>
#include <optional>

namespace pathfinder2 {
    struct PathPoint {
        Point point;
        bool is_optimal;
        std::optional<std::string> text;
    };

    class PathingAlgorithm {
    public:
        virtual ~PathingAlgorithm() {};
        PathingAlgorithm(const PathingAlgorithm &other) = delete;
        PathingAlgorithm &operator=(const PathingAlgorithm &other) = delete;
        virtual std::vector<PathPoint> find_path(const NodeMatrix &nodes) = 0;
    protected:
        PathingAlgorithm() = default;
    };

    class AStar : public PathingAlgorithm {
    public:
        AStar() = default;
        std::vector<PathPoint> find_path(const NodeMatrix &nodes) override;
    };
}
