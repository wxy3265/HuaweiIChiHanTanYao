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

    bool crashed;
private:
//    bool carrying;
    Point nextPoint;
    Goods goodsToGet;
    int mission;
    int targetId;
    int state = RobotState::FREE;
    bool enable;

    void move(int direction);
    void get();
    void pull();
    int waitTime = 0;
    Point lastPosition;
public:
    bool carrying;
    int id;
    Point position;
    int getState();
    Goods getGoodsToGet();
    void setMission(Goods _goodsToGet);
    void update();
    int getMission();
    int getTargetId();
    void redirection();
    void getNearRobot(Goods goods);
    void robotGetMission();
    void moveStep();
    void findBerth();
    void updateState(Point _position, bool _enable, bool _carrying);
};

extern Robot robot[10];
Path getPath1(int robId, Point target);
Path getPathbyAStar(int robId, Point target);
struct GoodsMission {
    Goods goods;
    double key;
    bool operator < (const GoodsMission &x) const{
        return key < x.key;
    }
};
extern bool nowFrameUse;
#endif //HUAWEIICHIHANTANYAO_ROBOT_H
