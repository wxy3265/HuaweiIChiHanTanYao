//
// Created by wxy3265 on 2024/3/7.
//

#include "Berth.h"

void Berth::pushGoods(Goods gd) {
    goods.push(gd);
}

Goods Berth::fetchGoods() {
    Goods goods1 = goods.front();
    goods.pop();
    return goods1;
}

void Berth::setShip(int st) {
    shipState = st;
}

int Berth::getShip() {
    return shipState;
}

bool Berth::empty() {
    return goods.empty();
}

int Berth::getGoodsNum() {
    return (int)goods.size();
}


