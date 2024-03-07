//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_STATE_H
#define HUAWEIICHIHANTANYAO_STATE_H


class BoatState {
public:
    static const int MOVING = 0;
    static const int PERFORMING = 1;
    static const int WAITING = 2;
};

class RobotState {
public:
    static const int RECOVERING = 0;
    static const int RUNNING = 1;
};

class BerthState {
public:
    static const int SHIP_PARKING = 0;
    static const int NO_SHIP_PARKING = 1;
};

class PointState {
public:
    static const char PLAIN = '.';
    static const char OCEAN = '*';
    static const char BLOCK = '#';
    static const char ROBOT = 'A';
    static const char BERTH = 'B';
};

class GoodsState {
public:
    static const int CARRYING = 1;
    static const int BERTH = 2;
    static const int SHIP = 3;
    static const int FREE = 0;
};

#endif //HUAWEIICHIHANTANYAO_STATE_H
