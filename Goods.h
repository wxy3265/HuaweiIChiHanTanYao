//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_GOODS_H
#define HUAWEIICHIHANTANYAO_GOODS_H

#include "Point.h"

class Goods {
private:
    int goodsState = GoodsState::FREE;
    int time;
public:
    int id;
    int value;
    Point position;
    Goods(const Point &position, int value, int time) : position(position), value(value), time(time) {}

    Goods() {}

    void setState(int state) {
        goodsState = state;
    }
};


#endif //HUAWEIICHIHANTANYAO_GOODS_H
