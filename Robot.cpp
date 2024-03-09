//
// Created by wxy3265 on 2024/3/7.
//

#include "Robot.h"

void Robot::move(int direction) {
    cout << "move " << id << ' ' << direction << '\n';
}
void Robot::get() {
    cout << "get " << id << '\n';
    cerr << "get!\n";
}
void Robot::pull() {
    cout << "pull " << id << '\n';
    cerr << "pull!\n";
}
int Robot::getState() {return state;}
vector<Goods> Robot::getGoods() {return goods;}
void Robot::setMission(Goods _goodsToGet, int _targetId) {
    cerr << id << "set" << targetId;
    goodsToGet = _goodsToGet;
    targetId = _targetId;
//    target.position.x += 3, target.position.y += 3;
    mission = RobotState::MISSION_GET;
    state = RobotState::MISSION_MOVE;
    nextPoint = position;
}

//更新机器人状态
void Robot::update(Point _position, bool _enable, bool _carrying) {
    if (true) {
        cerr << "robot" << id << "target:" << berth[targetId].position.x << ',' << berth[targetId].position.y << "pos:"
             << position.x << ' ' << position.y << "next: " << nextPoint.x << ',' << nextPoint.y
             << "path:" << robotPath[id].step << ' ' << robotPath[id].length << ';' << "gtg:" << goodsToGet.position.x << ','
             << goodsToGet.position.y << '\n';
    }
    position = _position;
    enable = _enable;
    carrying = _carrying;
//    while (true && frame != 1 && id >= 4) cerr << frame;
    if (state == RobotState::MISSION_MOVE) {
        robotGetGoods[id] = false;
        if (position == nextPoint) nextPoint = robotPath[id].getNextPoint();
        if (nextPoint == Point(-1, -1) || nextPoint == Point(0, 0) || robotPath[id].length == robotPath[id].step) {
            state = mission;
            return;
        }
        move(position.getDirection(nextPoint));
    } else if (state == RobotState::MISSION_GET) {
        get();
        if (!carrying) return;
        robotGetGoods[id] = true;
        goods.push_back(goodsToGet);
        nextPoint = robotPath[id].getNextPoint();
        state = RobotState::MISSION_MOVE;
        mission = RobotState::MISSION_PULL;
    } else if (state == RobotState::MISSION_PULL) {
        robotGetGoods[id] = false;
        pull();
        goods.pop_back();
        mission = state = RobotState::FREE;
    }
}

Robot robot[12];
