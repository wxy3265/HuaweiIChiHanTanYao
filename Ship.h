//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_SHIP_H
#define HUAWEIICHIHANTANYAO_SHIP_H

#include "Berth.h"

class ShipMission {
public:
    ShipMission(int targetId, int numToCarry);

    int targetId, numToCarry;
};

class Ship {
private:
    int mission = ShipState::FREE;
//    int targetId;
    queue<ShipMission> target;
    vector<Goods> goods;
    int state = ShipState::FREE;
    int endCompleteTime;
    bool firstMove = true;
    void autoSetMission();
public:
    int id;
    void get();
    void pull();
    int totValue();
    void setMission(ShipMission _target);
    void pushGoods(Goods goods1);
    bool isFree();
    int getState();
    vector<Goods> getGoods();
    void update(int _state);
};

extern Ship ship[7];
extern bool berthBanned[10];

#endif //HUAWEIICHIHANTANYAO_SHIP_H
