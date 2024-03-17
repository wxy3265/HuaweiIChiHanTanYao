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
    Point targetPosition;
    int distance;
    int velocity;
    int visitGoodsNumber = 0;
    void pushGoods(Goods gd);
    Goods fetchGoods();

    void setShip(int st);
    int getShip();
    bool empty();
    int getGoodsNum();
    int getHeadGoodsValue(int k);
    int getTotalValue();
};

extern Berth berth[10];
extern bool visitBerth[10];
extern bool berthVisitable[10];

#endif //HUAWEIICHIHANTANYAO_BERTH_H
