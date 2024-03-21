//
//
// Created by wxy3265 on 2024/3/8.


#include "Ship.h"

//船只指令等待时间的帧数偏移量
const int deltaFrame = 10;

void Ship::get() {
    firstMove = false;
    if (cerrSwitch && cerrShip) cerr << "ship! get" << id << ' ' << target << '\n';
    cout << "ship " << id << ' ' << target << '\n';
}

void Ship::pull() {
    if (cerrSwitch && cerrShip) cerr << "ship! pull" << id << '\n';
    cout << "go " << id << '\n';
}

void Ship::setMission(ShipMission _target) {
    if (cerrSwitch && cerrShip) cerr << "Ship[" << id << "] set mission:" << target << '\n';
    target = _target.targetId;
    mostVistedBerth++;
    visitBerth[_target.targetId] = true;
    startMissionTime = frame;
//    ZteChange = true;
    mission = ShipState::MISSION_MOVE;
    get();
}

void Ship::pushGoods(Goods goods1) {
    goods.push_back(goods1);
}
int Ship::getState() {return state;}

vector<Goods> Ship::getGoods() {return goods;}

void Ship::autoSetMission() {
    if (cerrSwitch && cerrShip) {
        cerr << "autoSetMission: [" << id << "]\n";
        for (int i = 0; i < 10; i++) {
            cerr << visitBerth[i] << ' ';
        }
        cerr << '\n';
    }
    if (frame + berth[target].distance + 500 >= 15000) {
        berthStateChange = true;
        berthOpenforShip[target] = false;
    }
    int mmax = 0, maxn = -1;
    int rest = capacity - goods.size();
    if(rest <= 13){
        back();
        return;
    }
    bool flag = false;
    for (int i = 0; i < 10; i++) {
        if (!berthOpenforShip[i]) continue;
        if (visitBerth[i]) continue;
        if (berth[i].getTotalValue() > mmax) {
            flag = true;
            if (berth[i].getGoodsNum() + goods.size() >= capacity || frame + berth[i].distance + 500 >= 15000 - deltaFrame) continue;
            if(goods.size() == 0 && frame + berth[i].distance * 2 + 50 >= 15000)continue;
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
    if (maxn != -1 ) {
        if (maxn == target) mission = ShipState::MISSION_GET;
        else setMission(ShipMission(maxn, -1));
    } else {
        mission = ShipState::FREE;
    }
}

int shipGetTotal = 0;
bool berthStateChange = false;

void Ship::update(int _state, int targetInput) {
//    while (true);
    if (cerrShip && cerrSwitch) {
        cerr << "ship:[" << id << "] totValue:<" << totValue() << "> goodsNumber:" << goods.size()
             << " target:" << target << " targetInput:" << targetInput << " nowframe:" << frame
             << " endF:" << startMissionTime << " mission:";
        if (mission == ShipState::MISSION_MOVE) cerr << "move";
        else if (mission == ShipState::MISSION_PULL) cerr << "pull";
        else if (mission == ShipState::MISSION_GET) cerr << "get";
        else if (mission == ShipState::FREE) cerr << "free";
        cerr << " state:";
        if (state == 0) cerr << "moving";
        else if (state == 2) cerr << "waiting";
        else if (state == 1) cerr << "working";
        cerr << " finalStage:" << finalStage;
        cerr << '\n';
    }
    if (frame == 1) {
        firstFinalTarget = id * 2, secondFinalTarget = id * 2 + 1;
    }
    state = _state;
    if (frame >= startMissionTime + 3 && targetInput != target) {
        if (cerrShip && cerrSwitch) cerr << "ship [" << id << "]: 不对！这不是我去的地方！\n";
        if (target != -1) get();
        else back();
        startMissionTime = frame;
        return;
    }
    int nowRestTime;
    if (target == -1) nowRestTime = 0;
    else nowRestTime = berth[target].distance;
    const int finalFrame = nowRestTime + berth[firstFinalTarget].distance + 50 + 500 +
                           50 + berth[secondFinalTarget].distance;
    if (frame >= 15000 - finalFrame - deltaFrame || finalStage > 1) {
        finalWork();
        return;
    }
    if (frame + berth[target].distance >= 15000 - deltaFrame && mission != ShipState::MISSION_PULL) {
        if (cerrShip && cerrSwitch) cerr << "ship:[" << id << "] 最终返回\n";
        berthOpenforShip[target] = false;
        berthStateChange = true;
        back();
        return;
    }
    if (mission == ShipState::MISSION_MOVE) {
        if (state == ShipState::PERFORMING && (frame >= startMissionTime + 50)) {
            mission = ShipState::MISSION_GET;
            startMissionTime = frame;
        }
    } else if (mission == ShipState::MISSION_GET) {
//        if (state == ShipState::PERFORMING) carryingGoodsNumber += berth[target].velocity;
        if (goods.size() >= capacity) {
            if (cerrShip && cerrSwitch) cerr << "ship:[" << id << "] 满载而归\n";
            back();
            return;
        }
//        if (frame >= 13500) {
//            get();
//            return;
//        }
        if (frame + berth[target].distance + 500 >= 15000 - deltaFrame) {
            if (cerrShip && cerrSwitch) cerr << "ship:[" << id << "] 临终等待\n";
            fetchGoods();
            return;
        }
        /*if (frame + 3500 >= 15000 - deltaFrame - 1) {
            if (cerrShip && cerrSwitch) cerr << "ship:[" << id << "] 最后调度\n";
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
        if (berth[target].empty() && (!firstMove || frame > startMissionTime + 50 || false)) {
            mission = ShipState::FREE;
            return;
        }
        fetchGoods();
    } else if (mission == ShipState::MISSION_PULL) {
        if (state == ShipState::PERFORMING && frame >= startMissionTime + 50) {
            goods.clear();
            mission = ShipState::FREE;
            carryingGoodsNumber = 0;
        }
    } else if (mission == ShipState::FREE) {
        if (mostVistedBerth >= mostBerthNumber) {
            back();
            return;
        }
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
    return ShipMission(target, numToCarry);
}

int Ship::getMission() {
    return mission;
}

void Ship::back() {
    if (frame + berth[target].distance + 500 >= 15000) {
        berthStateChange = true;
        berthOpenforShip[target] = false;
    }
    mostVistedBerth = 0;
    berthStateChange = true;
    pull();
    mission = ShipState::MISSION_PULL;
    startMissionTime = frame;
    target = -1;
}

void Ship::fetchGoods() {
    for (int i = 1; i <= berth[target].velocity && !berth[target].empty(); i++) {
        goods.push_back(berth[target].fetchGoods());
        shipGetTotal += goods.back().value;
//        if (cerrShip && cerrSwitch) cerr << "ship[" << id << "] fetched goods value<" << goods.back().value << "> tot:" << shipGetTotal << '\n';
    }
}

void Ship::finalWork() {
    if (finalStage == 1) { // start
        if (cerrSwitch && cerrShip) cerr << "于是，ship[" << id << "] 的小连招开始了！\n";
        back();
        finalStage = 2;
    } else if (finalStage == 2) { // moving to sell
        if (state == ShipState::PERFORMING && frame >= startMissionTime + 50) {
            setMission(ShipMission(firstFinalTarget, -1));
            carryingGoodsNumber = 0;
            finalStage = 3;
        }
    } else if (finalStage == 3) { // move to first target
        if (state == ShipState::PERFORMING && (frame >= startMissionTime + 50)) {
            mission = ShipState::MISSION_GET;
            startMissionTime = frame;
            finalStage = 4;
        }
    } else if (finalStage == 4) { // get from first target
        /*if (goods.size() >= capacity) {
            if (cerrShip && cerrSwitch) cerr << "ship:[" << id << "] 满载而归\n";
            back();
            return;
        }*/
        if (berth[target].empty() && (!firstMove || frame > startMissionTime + 50 || false)) {
            berthVisitable[target] = false;
            if (cerrSwitch && cerrShip) cerr << "关掉了[" << target << "]港口\n";
            setMission(ShipMission(secondFinalTarget, -1));
            finalStage = 5;
            berthStateChange = true;
            return;
        }
        fetchGoods();
    } else if (finalStage == 5) { // move to second target
        if (state == ShipState::PERFORMING && (frame >= startMissionTime + 50)) {
            mission = ShipState::MISSION_GET;
            startMissionTime = frame;
            finalStage = 6;
        }
    } else if (finalStage == 6) { // final waiting
        if (frame + berth[secondFinalTarget].distance + deltaFrame >= 15000) {
            back();
            finalStage = 7;
        }
    }
}

Ship ship[5];

ShipMission::ShipMission(int targetId, int numToCarry) : targetId(targetId), numToCarry(numToCarry) {}
