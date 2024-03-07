//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_ROBOT_H
#define HUAWEIICHIHANTANYAO_ROBOT_H

#include "tanyao.h"
#include "Goods.h"
#include "Point.h"
#include "State.h"

class Robot {
private:
    vector<Goods> goods;
    Point target;
    int state;
    void move(int direction) {
        cout << "move " << id << ' ' << direction << '\n';
    }
    void get() {
        cout << "get " << id << '\n';
    }
    void pull() {
        cout << "pull " << id << '\n';
    }
public:
    const int RIGHT = 0;
    const int LEFT = 1;
    const int UP = 2;
    const int DOWN = 3;

    int id;
    Point position;

    int getState() {return state;}
    vector<Goods> getGoods() {return goods;}

    void setMission(int tar) {
//        tar = ;
    }
    void update() {
        ;
    }
};


#endif //HUAWEIICHIHANTANYAO_ROBOT_H
