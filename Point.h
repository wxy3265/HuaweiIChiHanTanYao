//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_POINT_H
#define HUAWEIICHIHANTANYAO_POINT_H

class Point {
public:
    Point() {};
    Point(int x, int y) : x(x), y(y) {}
    bool operator != (Point b) {return x != b.x || y != b.y;}
public:
    int x, y;
};



#endif //HUAWEIICHIHANTANYAO_POINT_H
