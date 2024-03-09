//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_SHIP_H
#define HUAWEIICHIHANTANYAO_SHIP_H

#include "Berth.h"

class Ship {
private:
    int mission = ShipState::FREE;
    int targetId;
    vector<Goods> goods;
    int state = ShipState::FREE;
    int endCompleteTime;
public:
    int id;
    void get();
    void pull();

    void setMission(int _targetId);
    void pushGoods(Goods goods1);
    bool isFree();
    int getState();
    vector<Goods> getGoods();
    void update(int _state);
};

extern Ship ship[7];

#endif //HUAWEIICHIHANTANYAO_SHIP_H
