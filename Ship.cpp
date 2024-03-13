//
//
// Created by wxy3265 on 2024/3/8.


#include "Ship.h"

//船只指令等待时间的帧数偏移量
const int deltaFrame = 3;

void Ship::get() {
//    cerr << "ship! get" << id << ' ' << target.front().targetId << '\n';
    cout << "ship " << id << ' ' << target.front().targetId << '\n';
}

void Ship::pull() {
//    cerr << "ship! pull" << id << '\n';
    cout << "go " << id << '\n';
}

void Ship::setMission(ShipMission _target) {
    target.push(_target);
    endCompleteTime = frame + berth[target.front().targetId].distance + deltaFrame;
    get();
}

void Ship::pushGoods(Goods goods1) {
    goods.push_back(goods1);
}
int Ship::getState() {return state;}

vector<Goods> Ship::getGoods() {return goods;}

int shipGetTotal = 0;
bool berthBanned[10];

void Ship::update(int _state) {
//    while (true);
    if (true) {
        cerr << "ship:[" << id << "] totValue:<" << totValue() << "> goodsNumber:" << goods.size()
             << " target:" << target.front().targetId << " nowframe:" << frame << " endF:" << endCompleteTime
             << " todo:" << target.size() << " mission:";
        if (mission == ShipState::MISSION_MOVE) cerr << "move";
        else if (mission == ShipState::MISSION_PULL) cerr << "pull";
        else if (mission == ShipState::MISSION_GET) cerr << "get";
        else if (mission == ShipState::FREE) cerr << "free";
        cerr << " state:";
        if (state == 0) cerr << "moving";
        else if (state == 2) cerr << "waiting";
        else if (state == 1) cerr << "working";
        cerr << '\n';
    }
    state = _state;
//    if (state == ShipState::FREE) {
//        endCompleteTime++;
//        return;
//    }
    if (mission == ShipState::FREE) {
        if (!target.empty()) {
            mission = ShipState::MISSION_MOVE;
//            if (firstMove) endCompleteTime = 500, firstMove = false;
            get();
        }
    }
    if (mission == ShipState::MISSION_MOVE) {
        if (frame >= endCompleteTime) {
            mission = ShipState::MISSION_GET;
        }
    } else if (mission == ShipState::MISSION_GET) {
        if (goods.size() >= capacity) {
            cerr << "ship:[" << id << "] 满载而归\n";
            endCompleteTime = frame + berth[target.front().targetId].distance + deltaFrame;
            mission = ShipState::MISSION_PULL;
            while (!target.empty()) target.pop();
            pull();
            return;
        }
        if (frame + berth[target.front().targetId].distance >= 15000 - deltaFrame) {
            cerr << "ship:[" << id << "] 最终返回\n";
            mission = ShipState::MISSION_PULL;
            visitBerth[target.front().targetId] = false;
            endCompleteTime = frame + berth[target.front().targetId].distance;
            pull();
            berthVisitable[target.front().targetId] = false;
            berthBanned[target.front().targetId] = true;
            goods.clear();
            return;
        }
        if (frame + berth[target.front().targetId].distance + berth[(target.front().targetId + 1) % 10].distance + 500 >= 15000 - deltaFrame - 1) {
            cerr << "ship:[" << id << "] 临终等待\n";
            for (int i = 1; i <= berth[target.front().targetId].velocity && !berth[target.front().targetId].empty(); i++) {
                goods.push_back(berth[target.front().targetId].fetchGoods());
                shipGetTotal += goods.back().value;
            }
            return;
        }
        /*if (frame + berth[target.front().targetId].distance * 3 >= 15000 - deltaFrame - 1) {
            cerr << "ship:[" << id << "] 最后调度\n";
            for (int i = 0; i < 10; i++) {
                if (!visitBerth[i]) {
                    visitBerth[i] = true;
                    endCompleteTime = frame + 500 + deltaFrame;
                    mission = ShipState::MISSION_MOVE;
                    get();
                    return;
                }
            }
        }*/
        if (frame >= endCompleteTime + 100 &&
                ((berth[target.front().targetId].empty() && target.front().numToCarry == -1) ||
        goods.size() >= capacity ||
        (target.front().numToCarry != -1 && goods.size() >= target.front().numToCarry))) {
//            if (frame + berth[target.front().targetId].distance * 2 >= 14900) return;
//        if (frame - endCompleteTime > 100)  {
            visitBerth[target.front().targetId] = false;
            int dis = berth[target.front().targetId].distance;
            target.pop();
            if (!target.empty()) {
                endCompleteTime = frame + 500 + deltaFrame;
                mission = ShipState::MISSION_MOVE;
                get();
            } else {
                endCompleteTime = frame + dis + deltaFrame;
                mission = ShipState::MISSION_PULL;
                pull();
            }
            return;
        }
        for (int i = 1; i <= berth[target.front().targetId].velocity && !berth[target.front().targetId].empty(); i++) {
            goods.push_back(berth[target.front().targetId].fetchGoods());
            shipGetTotal += goods.back().value;
//            cerr << "ship[" << id << "] fetched goods value<" << goods.back().value << "> tot:" << shipGetTotal << '\n';
        }
    } else if (mission == ShipState::MISSION_PULL) {
        if (frame >= endCompleteTime) {
//            while (true) cerr << "empty!";
            mission = ShipState::FREE;
            goods.clear();
        }
    }
}

bool Ship::isFree() {
    return mission == ShipState::FREE;
}

int Ship::totValue() {
    int tot = 0;
    for (auto & good : goods) tot += good.value;
    return tot;
}

Ship ship[7];

ShipMission::ShipMission(int targetId, int numToCarry) : targetId(targetId), numToCarry(numToCarry) {}
