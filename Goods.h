//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_GOODS_H
#define HUAWEIICHIHANTANYAO_GOODS_H

#include "Point.h"
#include "State.h"

class Goods {
private:
    int goodsState = GoodsState::FREE;
    int value;
    int time;
public:
    Point position;
    Goods(const Point &position, int value, int time) : position(position), value(value), time(time) {}

    void setState(int state) {
        goodsState = state;
    }
};


#endif //HUAWEIICHIHANTANYAO_GOODS_H