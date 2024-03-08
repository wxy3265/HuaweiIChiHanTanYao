//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_SHIP_H
#define HUAWEIICHIHANTANYAO_SHIP_H

#include "Berth.h"

class Ship {
private:
    int mission = ShipState::FREE;
    Berth *target;
    vector<Goods> goods;
    int state = ShipState::FREE;
    int endCompleteTime;
    void get(Berth &berth);
    void sell();

public:
    int id;

    void setMission(Berth &berth);
    void pushGoods(Goods goods1);
    bool isFree();
    int getState();
    vector<Goods> getGoods();
    void update(int _state);
};


#endif //HUAWEIICHIHANTANYAO_SHIP_H
