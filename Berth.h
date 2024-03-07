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

    void pushGoods(Goods gd);
    vector<Goods> getGoods();

    void setShip(int st);
    int getShip();
};


#endif //HUAWEIICHIHANTANYAO_BERTH_H
