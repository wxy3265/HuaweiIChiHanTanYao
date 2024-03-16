//
//
// Created by wxy3265 on 2024/3/8.


#include "Ship.h"

//船只指令等待时间的帧数偏移量
const int deltaFrame = 1;

void Ship::get() {
//    cerr << "ship! get" << id << ' ' << target.front().targetId << '\n';
    cout << "ship " << id << ' ' << target.front().targetId << '\n';
}

void Ship::pull() {
//    cerr << "ship! pull" << id << '\n';
    cout << "go " << id << '\n';
}

void Ship::setMission(ShipMission _target) {
//    cerr << "Ship[" << id << "] set mission:" << _target.targetId << '\n';
    target.push(_target);
    visitBerth[_target.targetId] = true;
    startMissionTime = frame;
    if (mission == ShipState::FREE) {
        get();
        mission = ShipState::MISSION_MOVE;
    }
}

void Ship::pushGoods(Goods goods1) {
    goods.push_back(goods1);
}
int Ship::getState() {return state;}

vector<Goods> Ship::getGoods() {return goods;}

void Ship::autoSetMission() {
    cerr << "autoSetMission: [" << id << "]\n";
    for (int i = 0; i < 10; i++) {
        cerr << visitBerth[i] << ' ';
    }
    cerr << '\n';
    int mmax = 0, maxn = -1;
    bool flag = false;
    for (int i = 0; i < 10; i++) {
        if (visitBerth[i]) continue;
        if (berth[i].getTotalValue() > mmax) {
            flag = true;
            if (berth[i].getGoodsNum() + goods.size() >= capacity) continue;
            mmax = berth[i].getTotalValue();
            maxn = i;
        }
    }
    if (flag && maxn == -1) {
//        cerr << "ship:[" << id << "] 满载而归!\n";
        back();
        return;
    }
//    cerr << "cannot find max\n";
    if (maxn != -1) {
        setMission(ShipMission(maxn, -1));
    }
}

int shipGetTotal = 0;
bool berthBanned[10];

void Ship::update(int _state) {
//    while (true);
    if (cerrShip) {
        cerr << "ship:[" << id << "] totValue:<" << totValue() << "> goodsNumber:" << goods.size()
             << " target:" << target.front().targetId << " nowframe:" << frame << " endF:" << startMissionTime
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
    if (state == ShipState::FREE) {
        startMissionTime++;
        return;
    }
    if (mission == ShipState::FREE) {
        if (!target.empty()) {
            mission = ShipState::MISSION_MOVE;
//            if (firstMove) startMissionTime = 500, firstMove = false;
            get();
        }
    }
    if (mission == ShipState::MISSION_MOVE) {
        if (frame >= startMissionTime + 500 && state == ShipState::PERFORMING) {
            mission = ShipState::MISSION_GET;
            startMissionTime = frame;
        }
    } else if (mission == ShipState::MISSION_GET) {
        if (goods.size() >= capacity) {
//            cerr << "ship:[" << id << "] 满载而归\n";
            back();
            return;
        }
        if (!target.empty()) {
            if (frame + berth[target.front().targetId].distance >= 15000 - deltaFrame) {
                cerr << "ship:[" << id << "] 最终返回\n";
                back();
                berthVisitable[target.front().targetId] = false;
                berthBanned[target.front().targetId] = true;
                goods.clear();
                return;
            }
            if (frame + berth[target.front().targetId].distance + 500 >= 15000 - deltaFrame) {
//            cerr << "ship:[" << id << "] 临终等待\n";
                fetchGoods();
                return;
            }
        }
        if (!target.empty()) {
            autoSetMission();
            return;
        }
        /*if (frame + 3500 >= 15000 - deltaFrame - 1) {
            cerr << "ship:[" << id << "] 最后调度\n";
            for (int i = 0; i < 10; i++) {
                if (!visitBerth[i]) {
                    visitBerth[i] = true;
                    startMissionTime = frame + 500 + deltaFrame;
                    mission = ShipState::MISSION_MOVE;
                    get();
                    return;
                }
            }
        }*/
        if (frame >= startMissionTime + 100 &&
            ((berth[target.front().targetId].empty() && target.front().numToCarry == -1) ||
        (target.front().numToCarry != -1 && goods.size() >= target.front().numToCarry))) {
//            if (frame + berth[target.front().targetId].distance * 2 >= 14900) return;
            int dis = berth[target.front().targetId].distance;
            target.pop();
            if (!target.empty()) {
                cerr << "还有任务！" << '\n';
                startMissionTime = frame;
                mission = ShipState::MISSION_MOVE;
                get();
            } else {
                autoSetMission();
            }
            return;
        }
        fetchGoods();
    } else if (mission == ShipState::MISSION_PULL) {
        if (frame >= startMissionTime + 500 && state == ShipState::PERFORMING) {
//            while (true) cerr << "empty!";
            mission = ShipState::FREE;
        }
    } else if (mission == ShipState::FREE) {
        autoSetMission();
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

ShipMission Ship::getFirstTarget() {
    return target.front();
}

int Ship::getMission() {
    return mission;
}

void Ship::back() {
    pull();
    mission = ShipState::MISSION_PULL;
    startMissionTime = frame;
    goods.clear();
    while (!target.empty()) target.pop();
}

void Ship::fetchGoods() {
    for (int i = 1; i <= berth[target.front().targetId].velocity && !berth[target.front().targetId].empty(); i++) {
        goods.push_back(berth[target.front().targetId].fetchGoods());
        shipGetTotal += goods.back().value;
//            cerr << "ship[" << id << "] fetched goods value<" << goods.back().value << "> tot:" << shipGetTotal << '\n';
    }
}

Ship ship[5];

ShipMission::ShipMission(int targetId, int numToCarry) : targetId(targetId), numToCarry(numToCarry) {}
