//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_ROBOT_H
#define HUAWEIICHIHANTANYAO_ROBOT_H

#include "Path.h"
#include "Berth.h"

class Robot {
public:
    explicit Robot(const Berth &target);
    Robot() {}

private:
    bool carrying;
    Point nextPoint;
    Goods goodsToGet;
    int targetId;
    int mission;
    int state = RobotState::FREE;
    bool enable;
    bool crashed;
    void move(int direction);
    void get();
    void pull();
    int waitTime = 0;
    Point lastPosition;
public:
    int id;
    Point position;
    int getState();
    Goods getGoodsToGet();
    void setMission(Goods _goodsToGet, int _targetId);
    void update(Point _position, bool _enable, bool _carrying);
    int getMission();
};

extern Robot robot[10];

#endif //HUAWEIICHIHANTANYAO_ROBOT_H
