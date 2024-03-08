//
// Created by wxy3265 on 2024/3/8.
//

#include "Ship.h"

void Ship::get(Berth berth) {
    cerr << "get\n";
    cout << "ship " << id << ' ' << berth.id << '\n';
}

void Ship::sell() {
    cerr << "sell\n";
    cout << "go " << id << '\n';
}

void Ship::setMission(Berth berth) {
//    while (true) cerr << "ShipSetMission:" << id << ' ' << berth.id << '\n';
    target = berth;
    mission = ShipState::MISSION_GET;
    get(berth);
}

void Ship::pushGoods(Goods goods1) {
    goods.push_back(goods1);
}

int Ship::getState() {return state;}
vector<Goods> Ship::getGoods() {return goods;}

void Ship::update(int _state) {
    state = _state;
    if (mission == ShipState::MISSION_GET && state == ShipState::PERFORMING) {
//        if ()
        mission = ShipState::MISSION_PULL;
        sell();
    }
    if (mission == ShipState::MISSION_PULL && state == ShipState::PERFORMING) {
//        cerr << "Free\n";
//        mission = ShipState::FREE;
    }
}

bool Ship::isFree() {
    return mission == ShipState::FREE;
}
