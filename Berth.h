//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_BERTH_H
#define HUAWEIICHIHANTANYAO_BERTH_H

#include "tanyao.h"
#include "Goods.h"

class Berth {
private:
    int shipState;
    vector<Goods> goods;
public:
    int id;
    Point position;
    int distance;
    int velocity;

    void pushGoods(Goods gd) {
        goods.push_back(gd);
    }
    vector<Goods> getGoods() {return goods;}

    void setShip(int st) {
        shipState = st;
    }
    int getShip() {return shipState;}
};


#endif //HUAWEIICHIHANTANYAO_BERTH_H
