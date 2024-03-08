//
// Created by wxy3265 on 2024/3/8.
//

#include "Ship.h"

void Ship::get(Berth &berth) {
    cerr << "ship! get" << id << ' ' << berth.id << '\n';
    cout << "ship " << id << ' ' << berth.id << '\n';
}

void Ship::sell() {
    cerr << "ship! sell" << id << '\n';
    cout << "go " << id << '\n';
}

void Ship::setMission(Berth &berth) {
    get(berth);
    target = &berth;
    mission = ShipState::MISSION_MOVE;
    endCompleteTime = frame + berth.distance + 3;
}

void Ship::pushGoods(Goods goods1) {
    goods.push_back(goods1);
}

int Ship::getState() {return state;}
vector<Goods> Ship::getGoods() {return goods;}

void Ship::update(int _state) {
    state = _state;
    if (mission == ShipState::MISSION_MOVE) {
        if (frame >= endCompleteTime) {
            mission = ShipState::MISSION_GET;
        }
    } else if (mission == ShipState::MISSION_GET) {
        if (target->empty()) {
            sell();
            endCompleteTime = frame + target->distance + 3;
            mission = ShipState::MISSION_PULL;
        } else {
            goods.push_back(target->fetchGoods());
        }
    } else if (mission == ShipState::MISSION_PULL) {
        mission = ShipState::FREE;
    }
}

bool Ship::isFree() {
    return mission == ShipState::FREE;
}
