//
// Created by wxy3265 on 2024/3/8.
//

#include "Ship.h"

void Ship::get() {
    cerr << "ship! get" << id << ' ' << targetId << '\n';
    cout << "ship " << id << ' ' << targetId << '\n';
}

void Ship::pull() {
    cerr << "ship! pull" << id << '\n';
    cout << "go " << id << '\n';
}

void Ship::setMission(int _targetId) {
    get();
    targetId = _targetId;
    mission = ShipState::MISSION_MOVE;
    endCompleteTime = frame + berth[targetId].distance + 3;
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
    if (mission == ShipState::MISSION_MOVE) {
        if (frame >= endCompleteTime) {
            mission = ShipState::MISSION_GET;
        }
    } else if (mission == ShipState::MISSION_GET) {
        if (berth[targetId].empty() || goods.size() >= capacity) {
            endCompleteTime = frame + berth[targetId].distance + 3;
            mission = ShipState::MISSION_PULL;
            pull();
        }
        for (int i = 1; i <= berth[targetId].velocity && !berth[targetId].empty(); i++)
            goods.push_back(berth[targetId].fetchGoods());
        visitBerth[targetId] = false;
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
