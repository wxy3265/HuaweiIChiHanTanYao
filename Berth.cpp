//
// Created by wxy3265 on 2024/3/7.
//

#include "Berth.h"

void Berth::pushGoods(Goods gd) {
    goods.push_back(gd);
}

vector<Goods> Berth::getGoods() {
    return goods;
}

void Berth::setShip(int st) {
    shipState = st;
}

int Berth::getShip() {
    return shipState;
}
