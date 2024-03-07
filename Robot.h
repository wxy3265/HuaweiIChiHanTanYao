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
    void move(int direction) {
        cout << "move " << id << ' ' << direction << '\n';
    }
    void get() {
        cout << "get " << id << '\n';
    }
    void pull() {
        cout << "pull " << id << '\n';
    }
public:
    int id;
    Point position;

    int getState() {return state;}
    vector<Goods> getGoods() {return goods;}

    void setMission(Point _goodsToGet, Point _target) {
        goodsToGet = _goodsToGet;
        target = _target;
        mission = RobotState::MISSION_GET;
        path = Path(position, goodsToGet);
    }
    void update(Point _position, bool _enable) {
        position = _position;
        enable = _enable;
        if (state == RobotState::MISSION_MOVE) {
            Point nextPoint = path.getNextPoint();
            if (nextPoint == Point(-1, -1)) {
                state = mission;
            } else {
                move(position.getDirection(nextPoint));
            }
        } else if (state == RobotState::MISSION_GET) {
            get();
            path = Path(position, target);
            state = RobotState::MISSION_MOVE;
            mission = RobotState::MISSION_PULL;
        } else if (state == RobotState::MISSION_PULL) {
            pull();
            mission = state = RobotState::FREE;
        }
    }
};


#endif //HUAWEIICHIHANTANYAO_ROBOT_H
