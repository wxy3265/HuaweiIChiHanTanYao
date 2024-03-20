//
// Created by wxy3265 on 2024/3/7.
//

#include "Berth.h"

int totInValue = 0;
void Berth::pushGoods(Goods gd) {
    goods.push(gd);
    totInValue += gd.value;
    if (cerrSwitch && cerrTotalGetValue) cerr << "totInvalue:" << totInValue << '\n';
}

Goods Berth::fetchGoods() {
    Goods goods1 = goods.front();
    goods.pop();
    if(visitGoodsNumber > 0) visitGoodsNumber --;
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
    if (empty()) return 0;
//    cerr << "visitGoodsNumber:" << visitGoodsNumber << '\n';
    return (int)(goods.size());
}

int Berth::getHeadGoodsValue(int k) {
    queue <Goods> tmp;
    int n = goods.size(), tot = 0;
    for (int i = 1; i <= n && !goods.empty(); i++) {
        if (i <= k + visitGoodsNumber && i > visitGoodsNumber) tot += goods.front().value;
        tmp.push(goods.front());
        goods.pop();
    }
    while (!tmp.empty()) {
        goods.push(tmp.front());
        tmp.pop();
    }
    return tot;
}

int Berth::getTotalValue() {
    queue <Goods> tmp;
    int tot = 0;
    while(!goods.empty()) {
        tot += goods.front().value;
        tmp.push(goods.front());
        goods.pop();
    }
    while (!tmp.empty()) {
        goods.push(tmp.front());
        tmp.pop();
    }
    return tot;
}

Berth berth[10];
bool visitBerth[10];
bool berthVisitable[10];
