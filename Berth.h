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
    queue<Goods> goods;
public:
    int id;
    Point position;
    int distance;
    int velocity;

    void pushGoods(Goods gd);
    Goods fetchGoods();

    void setShip(int st);
    int getShip();
    bool empty();
    int getGoodsNum();
    int getHeadGoodsValue(int k);
};

extern Berth berth[12];
extern bool visitBerth[12];

#endif //HUAWEIICHIHANTANYAO_BERTH_H
