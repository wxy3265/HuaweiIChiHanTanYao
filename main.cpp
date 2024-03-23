#include "Map.h"

int robotHome[10];
int berthRobot[10];
int setedShipMissionTimes[10];
bool robotFirstMission[10];
int nearRobotId[200000];
int nearRobotDis[200000];
struct Operation{
    Goods targetGoods;
    int targetBerthId;
    double totalDistance;
    double goodsDistance;
    double efficiency;
    bool operator<(const Operation &x) const{
        return efficiency < x.efficiency;
    }
};
priority_queue<Operation> operation[12];
priority_queue<Operation> operationPlus[10];

struct Distance_to_berth{
    int berthId;
    int robotId;
    int distance;
}toBerthDistance[107];

struct GoodsMission {
    Goods goods;
    double key;
    bool operator < (const GoodsMission &x) const{
        return key < x.key;
    }
};

bool cmp(Distance_to_berth x, Distance_to_berth y){
    return x.distance < y.distance;
}

void printLength(int startberth);
void allocateHome();
void reallocateHome(int robId);
void robotSetMission(int robId, Goods goodsToGet, int targetBerthId);
void calcEfficiencyMax(int startBerthId);
void robotGetMission(int robId);
void robotGetMissionFromOperation(int robId);
void cerrBerthFun();
void checkBerthBanned();
void initShipMission();

int main() {
//    PathAlgorithm = 1;
    freopen("out3_19.txt", "w", stderr);
    memset(nearRobotId, -1, sizeof nearRobotId);
    Map::init();
    if (cerrSwitch) cerr << "capacity:" << capacity << '\n';
    Map::calcDistanceBetweenBerth();
    for(int i = 0; i <= 9; i++) Map::pretreatPathToStart(i), robotFirstMission[i] = true;
    for(int i = 0; i <= 9; i++) {
//        if (berthVisitable[i])
            Map::pretreatPathToBerth(i);
    }
    allocateHome();
//    for (int i = 0; i < 10; i++) cerr << i << "'s home:" << robotHome[i] << '\n';
    cout.flush();
    initShipMission();
    while (frame < 15000){
        if (frame >= 0) mostBerthNumber = 10000;
        if (frame >= 10000) mostBerthNumber = 4;
//        if (cerrSwitch && cerrFrame) cerr << "frame:" << frame << '\n';
        for (int i = 0; i < 10; i++) {
            visitBerth[i] = false;
        }
        for (int i = 0; i < 5; i++) {
            if (ship[i].getMission() == ShipState::MISSION_MOVE || ship[i].getMission() == ShipState::MISSION_GET) {
                visitBerth[ship[i].getFirstTarget().targetId] = true;
            }
        }
        Map::update();
//        if (frame >= 12500) {
//            berthOpenforShip[0] = true;
//            berthOpenforShip[1] = false;
//            berthOpenforShip[2] = true;
//            berthOpenforShip[3] = true;
//            berthOpenforShip[4] = false;
//            berthOpenforShip[5] = false;
//            berthOpenforShip[6] = true;
//            berthOpenforShip[7] = true;
//            berthOpenforShip[8] = false;
//            berthOpenforShip[9] = false;
////            berthOpenforShip[0] = {true, false, true, true, false, false, true, true, false, false}
////            for (int i = 0; i < 9; i++) berthOpenforShip[i] = false;
////            for (int i = 0; i < 5; i++) {
////                int targetId = ship[i].getFirstTarget().targetId;
////                if (targetId == -1) continue;
////                berthOpenforShip[targetId] = true;
////            }
//        }
//        if (frame >= 10000) {
//            berthVisitable[0] = true;
//            berthVisitable[1] = false;
//            berthVisitable[2] = true;
//            berthVisitable[3] = true;
//            berthVisitable[4] = false;
//            berthVisitable[5] = false;
//            berthVisitable[6] = true;
//            berthVisitable[7] = true;
//            berthVisitable[8] = false;
//            berthVisitable[9] = false;
//            Map::initNear();
//            for (int j = 0; j <= 9; j++) {
//                bool existTarget = false;
//                for (int i = 0; i < 5; i++) {
//                    if (ship[i].getFirstTarget().targetId == j) existTarget = true;
//                }
//                if (!berthVisitable[j]) continue;
//                Map::pretreatPathToBerth(j);
//            }
//            for (int i = 0; i < 9; i++) reallocateHome(i);
//        }
        if (frame == 1) initShipMission();
        if (cerrSwitch && cerrBerth) {
            cerrBerthFun();
        }
        checkBerthBanned();
        if (!bindBerth) {
            for (int i = 0; i <= 9; i++) {
                if (robot[i].getState() == RobotState::FREE)
                    robotGetMission(i);
                else if (robot[i].getState() == RobotState::MISSION_MOVE) {
                    if (robot[i].getMission() == RobotState::MISSION_GET && robot[i].carrying == 1 && robotPath[i].step == 3) {
                        if (cerrSwitch) cerr << "rediretction" << i << "\n";
                        robot[i].redirection();
                    }
                }

            }
        } else {
            for (int i = 0; i <= 9; i++) calcEfficiencyMax(i);
            while (!newGoods.empty()) newGoods.pop_back();
            for (int i = 0; i <= 9; i++) {
                if (robot[i].getState() == RobotState::FREE)
                    robotGetMissionFromOperation(i);
                else if (robot[i].getState() == RobotState::MISSION_MOVE) {
                    if (robot[i].getMission() == RobotState::MISSION_GET && robot[i].carrying == 1 && robotPath[i].step == 3) {
                        if (cerrSwitch) cerr << "rediretction" << i << "\n";
                        robot[i].redirection();
                    }
                }

            }
        }
        cout << "OK" << endl;
        cout.flush();
    }
    if (cerrSwitch) cerr << "跳帧数量" << framesum << "\n";
    return 0;
}

void initShipMission() {
//    for (int i = 0; i < 5; i++) ship[i].setMission(ShipMission{i * 2, -1});
    ship[0].setMission(ShipMission{0, -1});
    ship[1].setMission(ShipMission{1, -1});
    ship[2].setMission(ShipMission{4, -1});
    ship[3].setMission(ShipMission{5, -1});
    ship[4].setMission(ShipMission(6, -1));
}
void getNearRobot(Goods goods){
    if(nearRobotId[goods.id] >= 0 || nearRobotId[goods.id] == -2)return;
    int mindis = 100000, nearId = -2, nowId;
    for(int i = 0; i <= 9; i++){
        if (robotFirstMission[i])nowId = -2;
        else nowId = robot[i].getTargetId();
        if(robotHome[i] == -1||nowId == -1)continue;
        int dis;
        if(nowId == -2)dis = Map::getLengthFromStartToPoint(i, goods.position);
        else dis = Map::getLengthFromBerthToPoint(nowId,goods.position);
        if(dis < mindis){
            mindis = dis;
            nearId = i;
        }
    }
    nearRobotId[goods.id] = nearId;
    nearRobotDis[goods.id] = mindis;
}
void robotGetMission(int robId) {
    if (robotHome[robId] == -1) return;
    int nowBerthId;
    if (robotFirstMission[robId]) nowBerthId = -2;
    else nowBerthId = robot[robId].getTargetId();
    if (nowBerthId == -1) return;
    int n = goodsOnMap.size();
//    for (int i = 0; i < n; i++) {
//        cerr << goodsOnMap[i].value << ' ';
//    }
//    cerr << '\n';
    priority_queue <GoodsMission> goodsMission;
//    cerr << "goodsOnMapSize:" << goodsOnMap.size() << '\n';
    for (int i = 0; i < n; i++) {
        Goods goods = goodsOnMap[i];
        GoodsMission goodsMissionNow;
////        if ((robId == 2 || robId == 3 || robId == 4) && (goods.position.x <= 100 || goods.position.y <= 101)) continue;
//        if ((robId >= 0 && robId <= 1) && (goods.position.x <= 6 || goods.position.x >= 52 || goods.position.y <= 3 || goods.position.y >= 94)) continue;
//        if ((robId >= 2 && robId <= 3) && (goods.position.x <= 6 || goods.position.x >= 52 || goods.position.y <= 109 || goods.position.y >= 199)) continue;
//        if ((robId >= 4 && robId <= 6) && (goods.position.x < 77 || goods.position.x > 124 || goods.position.y < 56 || goods.position.y > 146)) continue;
//        if ((robId >= 7 && robId <= 8) && (goods.position.x <= 127)) continue;
//        if ((robId == 9) && ) continue;
        int nearBerthId = Map::getNearBerthId(goods.position);
        if (nearBerthId == -1) continue;
//        int distance = Map::getLengthFromBerthToPoint(nearBerthId, goods.position);
        int distance = 0;
        if (nowBerthId == -2) distance += Map::getLengthFromStartToPoint(robId, goods.position);
        else distance += Map::getLengthFromBerthToPoint(nowBerthId, goods.position);
//        if (frame >= 1000 && frame + distance <= goods.time + 500) continue;
        if (visitGoods[goods.id]) continue;
        if (frame + distance + 20 >= goods.time + 1000) continue;
//        if (goods.value < 100) continue;
//        if (distance > 150) continue;
        bool existTarget = false;
        for (int i = 0; i < 5; i++) {
            if (ship[i].getFirstTarget().targetId == nearBerthId) existTarget = true;
        }        getNearRobot(goods);
        int mindis = nearRobotDis[goods.id];
        goodsMissionNow.goods = goods;
//        goodsMissionNow.key = -distance;
        goodsMissionNow.key =  100.0 * (goods.value - (distance - mindis) * deltaLength + existTarget * deltaTarget + berth[nearBerthId].getGoodsNum() * 0)  /
                                (distance + (goods.time + 1000 - frame) * deltaTime);
//        if (frame + distance + 25 >= goods.time + 900) goodsMissionNow.key += 1000;
        goodsMission.push(goodsMissionNow);
    }
    if (goodsMission.empty()) return;
    GoodsMission targetMission = goodsMission.top();
    int targetBerthId = Map::getNearBerthId(targetMission.goods.position);
//    cerr << "chose:" << targetMission.goods.value << '\n';
    robotSetMission(robId, targetMission.goods, targetBerthId);
}
void cerrBerthFun() {
    int totBerthValue = 0;
    for (int i = 0; i < 10; i++) {
//        if (!berthVisitable[i]) continue;
        cerr << "berth:[" << i << "] value:<" << berth[i].getTotalValue()
             << "> numbers:" << berth[i].getGoodsNum()
             << " distance:" << berth[i].distance
             << " visit:" << visitBerth[i]
             << " visitable:" << berthVisitable[i]
             << '\n';
        totBerthValue += berth[i].getTotalValue();
    }
    cerr << "totBerthValue: <" << totBerthValue << ">\n";
}
void checkBerthBanned() {
    if (berthStateChange) {
        Map::initNear();
        for (int j = 0; j <= 9; j++) {
            bool existTarget = false;
            for (int i = 0; i < 5; i++) {
                if (ship[i].getFirstTarget().targetId == j) existTarget = true;
            }
            if (!berthVisitable[j]) continue;
            Map::pretreatPathToBerth(j);
        }
        for (int i = 0; i < 10; i++) reallocateHome(i);
    }
    berthStateChange = false;
}
void robotSetMission(int robId, Goods goodsToGet, int targetBerthId) {
//    cerr << "robotSetMission tar:" << targetBerthId << '\n';
    visitGoods[goodsToGet.id] = true;
    robot[robId].setMission(goodsToGet, targetBerthId);
    robotFirstMission[robId] = false;
//    robotHome[robId] = targetBerthId;
}
void allocateHome(){
    for(int i = 0; i <= 9; i++) robotHome[i] = berthRobot[i] = -1;
    int cnt = 0;
    for(int i = 0; i <= 9; i++){
        for(int j = 0; j <= 9; j++){
            toBerthDistance[++cnt].robotId = i;
            toBerthDistance[cnt].berthId = j;
            //toBerthDistance[cnt].distance = Path(robot[i].position, berth[j].position, -1).length;
            toBerthDistance[cnt].distance = Map::getLengthFromBerthToPoint(j, robot[i].position);
        }
    }
    sort(toBerthDistance + 1, toBerthDistance + cnt + 1, cmp);

    for(int i = 1; i <= cnt; i++){
        if(toBerthDistance[i].distance > 100000)continue;
        if(!Map::isOpen(toBerthDistance[i].berthId))continue;
        if(robotHome[toBerthDistance[i].robotId] != -1)continue;
        if(berthRobot[toBerthDistance[i].berthId] != -1)continue;
        robotHome[toBerthDistance[i].robotId] = toBerthDistance[i].berthId;
        berthRobot[toBerthDistance[i].berthId] = toBerthDistance[i].robotId;
    }
    for(int i = 1; i <= cnt; i++){
        if(toBerthDistance[i].distance > 100000)continue;
        if(!Map::isOpen(toBerthDistance[i].berthId))continue;
        if(robotHome[toBerthDistance[i].robotId] != -1)continue;
        robotHome[toBerthDistance[i].robotId] = toBerthDistance[i].berthId;
    }
}
void reallocateHome(int robId) {
    int mindis = 100000;
    int home = -1;
    for(int i = 0; i <= 9; i++){
        if(!berthVisitable[i])continue;
        int dis = Map::getLengthFromBerthToPoint(i,robot[robId].position);
        if(dis < mindis){
            mindis = dis;
            home = i;
        }
    }
    if(home != -1)robotHome[robId] = home;
}
void robotGetMissionFromOperation(int robId) {
//    cerr << "robGetPlusID:[" << robId << "]" << operation[robotHome[robId]].size() << "\n";
    if (robotHome[robId] == -1) return;
    while(!operation[robotHome[robId]].empty()){
        double dis = operation[robotHome[robId]].top().goodsDistance;
        int time = operation[robotHome[robId]].top().targetGoods.time;
        if((frame + dis + 20 < time + 1000)
           && !visitGoods[operation[robotHome[robId]].top().targetGoods.id]
           && operation[robotHome[robId]].top().goodsDistance <= 150) {
            break;
        }
        operation[robotHome[robId]].pop();
    }
    if(operation[robotHome[robId]].empty()) return;
    robotSetMission(robId, operation[robotHome[robId]].top().targetGoods,
                    operation[robotHome[robId]].top().targetBerthId);
    operation[robotHome[robId]].pop();
}
void calcEfficiencyMax(int startBerthId) {
//    double deltaLength = 0.56;
//    double deltaTime = 0.5;
    for(auto & newGood : newGoods) {
        int nearBerthId = Map::getNearBerthId(newGood.position);
        if (nearBerthId == -1) continue;
        //if(nearBerthId != startBerthId)continue;
        double pathLength = Map::getLengthFromBerthToPoint(nearBerthId, newGood.position) * 2;
        double pathLength1 = Map::getLengthFromBerthToPoint(startBerthId, newGood.position) * 2;
        double goodsTime = newGood.time;
        double efficiency = 100.0 * newGood.value / pathLength1;
        double efficiency1 = 100.0 * (newGood.value - (pathLength1 - pathLength) * deltaLength)  /
                             (pathLength1 + (goodsTime + 1000 - frame) * deltaTime);
//        efficiency1 = 100.0 / pathLength1;
//        if (newGood.value < 100) continue;
//        if (pathLength1 >= 150) continue;
        //operation[nearBerthIds].push((Operation){newGood, nearBerthId, pathLength, pathLength / 2.0, efficiency});
        //if(nearBerthId != startBerthId)
//        if ((startBerthId == 6 || startBerthId == 8 || startBerthId == 4 || startBerthId == 7) && (newGood.position.x <= 100 || newGood.position.y <= 100)) continue;
        operation[startBerthId].push((Operation){newGood,startBerthId,pathLength1,
                                                 pathLength1 / 2.0, efficiency1});
    }
//    checkOperation(startBerthId);
}
