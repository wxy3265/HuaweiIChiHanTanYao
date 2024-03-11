//
// Created by 86187 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_MAP_H
#define HUAWEIICHIHANTANYAO_MAP_H
#include "Ship.h"
#include "Robot.h"

class Map {
public:
    static void init();

    static void update();

    static void pretreatPath(int berthId);

    static int getLengthFromBerthToPoint(int berthId, Point end);

    static void calcDistanceBetweenBerth();

    static bool isOpen(int id);

    static int getNearBerthId(Point point);
};

#endif //HUAWEIICHIHANTANYAO_MAP_H
