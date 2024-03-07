//
// Created by wxy3265 on 2024/3/7.
//

#include "Robot.h"

void Robot::move(int direction) {
    cout << "move " << id << ' ' << direction << '\n';
}
void Robot::get() {
    cout << "get " << id << '\n';
}
void Robot::pull() {
    cout << "pull " << id << '\n';
}
int Robot::getState() {return state;}
vector<Goods> Robot::getGoods() {return goods;}
void Robot::setMission(Point _goodsToGet, Point _target) {
    goodsToGet = _goodsToGet;
    target = _target;
    mission = RobotState::MISSION_GET;
    state = RobotState::MISSION_MOVE;
    path = Path(position, goodsToGet, id);
    nextPoint = position;
}
void Robot::update(Point _position, bool _enable) {
    position = _position;
    enable = _enable;
    if (true) cerr << "pos:" << position.x << ' ' << position.y << " state:" << state << "next:" << nextPoint.x << ',' << nextPoint.y << '\n';
//    while (position == nextPoint) ;
    if (state == RobotState::MISSION_MOVE) {
        if (position == nextPoint) nextPoint = path.getNextPoint();
        if (nextPoint == Point(-1, -1)) {
            state = mission;
            return;
        }
        move(position.getDirection(nextPoint));
    } else if (state == RobotState::MISSION_GET) {
        get();
        path = Path(position, target, id);
        state = RobotState::MISSION_MOVE;
        mission = RobotState::MISSION_PULL;
    } else if (state == RobotState::MISSION_PULL) {
        pull();
        mission = state = RobotState::FREE;
    }
}
