//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_SHIP_H
#define HUAWEIICHIHANTANYAO_SHIP_H

#include "tanyao.h"
#include "Goods.h"
#include "Berth.h"

class Ship {
private:
    Berth target;
    vector<Goods> goods;
    int state;
public:
    static int capacity;
    int id;

    void setMission(Berth berth) {
        target = berth;
    }
    void pushGoods(Goods goods1) {
        goods.push_back(goods1);
    }

    int getState() {return state;}
    vector<Goods> getGoods() {return goods;}
};


#endif //HUAWEIICHIHANTANYAO_SHIP_H
