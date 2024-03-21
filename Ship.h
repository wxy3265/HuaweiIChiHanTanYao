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
    int target = -1;
    int carryingGoodsNumber = 0;
    int numToCarry;
    vector<Goods> goods;
    int state = ShipState::FREE;
    int finalStage = 1;
    int firstFinalTarget, secondFinalTarget;
    int startMissionTime;
    bool firstMove = true;
    void autoSetMission();
    void back();
    void fetchGoods();
    void finalWork();
public:
    int id;
    int mostVistedBerth;
    void get();
    void pull();
    int totValue();
    void setMission(ShipMission _target);
    void pushGoods(Goods goods1);
    bool isFree();
    int getMission();
    int getState();
    vector<Goods> getGoods();
    ShipMission getFirstTarget();
    void update(int _state, int targetInput);
};

extern Ship ship[5];
extern bool berthStateChange;

#endif //HUAWEIICHIHANTANYAO_SHIP_H
