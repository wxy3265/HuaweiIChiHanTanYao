//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_POINT_H
#define HUAWEIICHIHANTANYAO_POINT_H

#include "State.h"

class Point {
public:
    Point() {};

    int x, y;
    Point(int x, int y) : x(x), y(y) {}
    bool operator != (Point b) const {return x != b.x || y != b.y;}
    bool operator == (Point b) const {return x == b.x && y == b.y;}

    int getDirection(Point b) {
        if (y > b.y) return Direction::LEFT;
        if (y < b.y) return Direction::RIGHT;
        if (x > b.x) return Direction::UP;
        return Direction::DOWN;
    }
};



#endif //HUAWEIICHIHANTANYAO_POINT_H
