//
// Created by wxy3265 on 2024/3/7.
//

#include "Robot.h"

void Robot::move(int direction) {
    cout << "move " << id << ' ' << direction << '\n';
}
void Robot::get() {
    cout << "get " << id << '\n';
//    cerr << "get!\n";
}
void Robot::pull() {
    cout << "pull " << id << '\n';
//    cerr << "pull!\n";
}
int Robot::getState() {return state;}
void Robot::setMission(Goods _goodsToGet, int _targetId) {
    goodsToGet = _goodsToGet;
    targetId = _targetId;
//    target.position.x += 3, target.position.y += 3;
    mission = RobotState::MISSION_GET;
    state = RobotState::MISSION_MOVE;
    nextPoint = position;
//    cerr << id << "set" << _targetId;
}

int totGetValue = 0;
//更新机器人状态
void Robot::update(Point _position, bool _enable, bool _carrying) {
    robotCrushed[id] = false;
//    if (id == 6) {
//        cerr << "robot" << id << "target:" << berth[targetId].position.x << ',' << berth[targetId].position.y << "pos:"
//             << position.x << ' ' << position.y << "next: " << nextPoint.x << ',' << nextPoint.y
//             << "path:" << robotPath[id].step << ' ' << robotPath[id].length << ';'
//             << "gtg:" << goodsToGet.position.x << ',' << goodsToGet.position.y
//             << "carrying:" << carrying << " enable:" << enable << " crash:" << crashed << '\n';
//    }
    position = _position;
    enable = _enable;
    carrying = _carrying;
//    while (true && frame != 1 && id >= 4) cerr << frame;
    if (!enable) {
//        robotPath[id] = Path();
        crashed = true;
        return;
    }
    if (state == RobotState::MISSION_MOVE) {
//        if (mission == RobotState::MISSION_PULL) cerr << "carrying:" << carrying << '\n';
        if (crashed) {
//            cerr << "crashed:" << id << '\n';
            robotCrushed[id] = true;
            crashed = false;
            return;
        }
        robotGetGoods[id] = false;
        if (position == nextPoint || nextPoint == Point(-1, -1)) {
            nextPoint = robotPath[id].getNextPoint();
//            cerr << "getNextPoint" << nextPoint.x << ',' << nextPoint.y << '\n';
        }
        if (nextPoint == Point(-1, -1) || robotPath[id].length == robotPath[id].step) {
            state = mission;
            return;
        }
        move(position.getDirection(nextPoint));
    } else if (state == RobotState::MISSION_GET) {
        get();
//        cerr << "get:" << position.x << ',' << position.y
//             << " goods:" << goodsToGet.position.x << ',' << goodsToGet.position.y << '\n';
        if (!carrying) {
//            cerr << "returned!\n";
            return;
        }
        totGetValue += goodsToGet.value;
//        cerr << "getedV:" << totGetValue << '\n';
        robotGetGoods[id] = true;
        nextPoint = robotPath[id].getNextPoint();
        state = RobotState::MISSION_MOVE;
        mission = RobotState::MISSION_PULL;
    } else if (state == RobotState::MISSION_PULL) {
//        cerr << id << "Pulled\n";
        pull();
        berth[targetId].pushGoods(goodsToGet);
        if (carrying) return;
        robotGetGoods[id] = false;
        mission = state = RobotState::FREE;
    }
}

int Robot::getMission() {
    return mission;
}

Goods Robot::getGoodsToGet() {
    return goodsToGet;
}

Robot robot[12];
