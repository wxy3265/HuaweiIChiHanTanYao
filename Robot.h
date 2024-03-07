//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_ROBOT_H
#define HUAWEIICHIHANTANYAO_ROBOT_H

#include "tanyao.h"
#include "Goods.h"
#include "Point.h"
#include "State.h"
#include "Path.h"

class Robot {
private:
    vector<Goods> goods;
    Point goodsToGet, target;
    int mission;
    Path path;
    int state = RobotState::FREE;
    bool enable;
    void move(int direction);
    void get();
    void pull();
public:
    int id;
    Point position;
    int getState();
    vector<Goods> getGoods();
    void setMission(Point _goodsToGet, Point _target);
    void update(Point _position, bool _enable);
};


#endif //HUAWEIICHIHANTANYAO_ROBOT_H
