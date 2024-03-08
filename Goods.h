//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_GOODS_H
#define HUAWEIICHIHANTANYAO_GOODS_H

#include "Point.h"

class Goods {
private:
    int goodsState = GoodsState::FREE;
public:
    int id;
    int value;
    int time;
    Point position;
    Goods(const Point &position, int value, int time, int id) : position(position), value(value), time(time), id(id){}

    Goods() {}

    void setState(int state) {
        goodsState = state;
    }
};


#endif //HUAWEIICHIHANTANYAO_GOODS_H
