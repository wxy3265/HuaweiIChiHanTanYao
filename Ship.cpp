//
// Created by wxy3265 on 2024/3/8.
//

#include "Ship.h"

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
    endCompleteTime = frame + berth[target.front().targetId].distance + 10;
}

void Ship::pushGoods(Goods goods1) {
    goods.push_back(goods1);
}

int Ship::getState() {return state;}
vector<Goods> Ship::getGoods() {return goods;}

int shipGetTotal = 0;

void Ship::update(int _state) {
//    cerr << "ship:" << id << " totValue:" << totValue() << " goodsNumber:" << goods.size()
//    << " nowframe:" << frame << " endF:" << endCompleteTime
//    << "targetSize:" << target.size() << " mission:";
//    if (mission == ShipState::MISSION_MOVE) cerr << "move";
//    else if (mission == ShipState::MISSION_PULL) cerr << "pull";
//    else if (mission == ShipState::MISSION_GET) cerr << "get";
//    else if (mission == ShipState::FREE) cerr << "free";
//    cerr << '\n';
    state = _state;
//    if (state == ShipState::FREE) {
//        endCompleteTime++;
//        return;
//    }
//    if (frame == 14999) cerr << "[" << id << "] remain " << "<" << totValue() << ">\n";
//    cerr << "frame:" << frame << "," << this->id << "LKP AK IOI" << "\n";
    if (mission == ShipState::FREE) {
        if (!target.empty()) {
            mission = ShipState::MISSION_MOVE;
            if (firstMove) endCompleteTime = frame, firstMove = false;
            get();
        }
    } if (mission == ShipState::MISSION_MOVE) {
        if (frame >= endCompleteTime) {
            mission = ShipState::MISSION_GET;
        }
    } else if (mission == ShipState::MISSION_GET) {
        if (frame + berth[target.front().targetId].distance >= 14995) {
            mission = ShipState::MISSION_PULL;
            visitBerth[target.front().targetId] = false;
            endCompleteTime = frame + berth[target.front().targetId].distance + 10;
            pull();
            return;
        }
        if ((berth[target.front().targetId].empty() && target.front().numToCarry == -1) ||
        goods.size() >= capacity ||
        (target.front().numToCarry != -1 && goods.size() >= target.front().numToCarry)) {
//        if (frame - endCompleteTime > 100)  {
//            cerr << "frame:" << frame << "," << this->id << "LKP AK IOI" << "\n";
            visitBerth[target.front().targetId] = false;
            int dis = berth[target.front().targetId].distance;
            target.pop();
//            cerr << "frame:" << frame << "," << this->id << "LKP AK IOI" << "\n";
            if (!target.empty()) {
                endCompleteTime = frame + 503;
                mission = ShipState::MISSION_MOVE;
            } else {
                endCompleteTime = frame + dis + 10;
                mission = ShipState::MISSION_PULL;
                pull();
            }
            return;
        }
//        cerr << target.front().targetId << "\n";
        for (int i = 1; i <= berth[target.front().targetId].velocity && !berth[target.front().targetId].empty(); i++) {
//        cerr << "frame:" << frame << "," << this->id << "LKP AK IOI" << "\n";
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
