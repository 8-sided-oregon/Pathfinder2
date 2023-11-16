#include "node.hpp"

using namespace pathfinder2;

Node &pathfinder2::operator++(Node &rhs) {
    switch (rhs) {
    case Node::Walkable:    return rhs = Node::Obstical; 
    case Node::Obstical:    return rhs = Node::Start;
    case Node::Start:       return rhs = Node::End;
    case Node::End:         return rhs = Node::Walkable;
    }
}

Node &pathfinder2::operator--(Node &rhs) {
    switch (rhs) {
    case Node::Walkable:    return rhs = Node::End; 
    case Node::Obstical:    return rhs = Node::Walkable;
    case Node::Start:       return rhs = Node::Obstical;
    case Node::End:         return rhs = Node::Start;
    }
}

float pathfinder2::dist(Point a, Point b) {
    int dx = a.first - b.first;
    int dy = a.second - b.second;
    return std::sqrt(dx * dx + dy * dy);
}

Point pathfinder2::operator+(Point a, Point b) {
    return {a.first + b.first, a.second + b.second};
}
