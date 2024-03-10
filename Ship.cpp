//
// Created by wxy3265 on 2024/3/8.
//

#include "Ship.h"

void Ship::get() {
    cerr << "ship! get" << id << ' ' << target.front().targetId << '\n';
    cout << "ship " << id << ' ' << target.front().targetId << '\n';
}

void Ship::pull() {
    cerr << "ship! pull" << id << '\n';
    cout << "go " << id << '\n';
}

void Ship::setMission(ShipMission _target) {
    target.push(_target);
    mission = ShipState::MISSION_MOVE;
    endCompleteTime = frame + berth[target.front().targetId].distance + 3;
    if (frame <= 3) endCompleteTime = 0;
}

void Ship::pushGoods(Goods goods1) {
    goods.push_back(goods1);
}

int Ship::getState() {return state;}
vector<Goods> Ship::getGoods() {return goods;}

void Ship::update(int _state) {
    cerr << "ship:" << id << " mission:" << mission << " nowframe:" << frame
         << "endF:" << endCompleteTime << '\n';
    state = _state;
//    if (state == ShipState::FREE) {
//        endCompleteTime++;
//        return;
//    }
    if (mission == ShipState::FREE) {
        if (!target.empty()) {
            mission = ShipState::MISSION_MOVE;
        }
    } if (mission == ShipState::MISSION_MOVE) {
        if (frame >= endCompleteTime) {
            mission = ShipState::MISSION_GET;
        }
    } else if (mission == ShipState::MISSION_GET) {
        if (berth[target.front().targetId].empty() || goods.size() >= capacity ||
        (target.front().numToCarry != -1 && goods.size() >= target.front().numToCarry)) {
            if (!target.empty()) {
                endCompleteTime = frame + 503;
                mission = ShipState::MISSION_MOVE;
                target.pop();
            } else {
                endCompleteTime = frame + berth[target.front().targetId].distance + 3;
                mission = ShipState::MISSION_PULL;
                pull();
            }
        }
        for (int i = 1; i <= berth[target.front().targetId].velocity && !berth[target.front().targetId].empty(); i++)
            goods.push_back(berth[target.front().targetId].fetchGoods());
        visitBerth[target.front().targetId] = false;
    } else if (mission == ShipState::MISSION_PULL) {
        if (frame >= endCompleteTime) {
//            while (true) cerr << "empty!";
            mission = ShipState::FREE;
            goods.clear();
        }
    }
}

bool Ship::isFree() {
    return mission == ShipState::FREE;
}

Ship ship[7];

ShipMission::ShipMission(int targetId, int numToCarry) : targetId(targetId), numToCarry(numToCarry) {}
