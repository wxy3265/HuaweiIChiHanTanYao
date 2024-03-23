#include "Map.h"

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

bool cmp(Distance_to_berth x, Distance_to_berth y){
    return x.distance < y.distance;
}

void printLength(int startberth);
void allocateHome();
void reallocateHome(int robId);
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
        if (cerrSwitch && cerrFrame) cerr << "frame:" << frame << '\n';
        for (int i = 0; i < 10; i++) {
            visitBerth[i] = false;
        }
        for (int i = 0; i < 5; i++) {
            if (ship[i].getMission() == ShipState::MISSION_MOVE || ship[i].getMission() == ShipState::MISSION_GET) {
                visitBerth[ship[i].getFirstTarget().targetId] = true;
            }
        }
        Map::update();
        for (int i = 0; i < 10; i++) robot[i].update();
        if (frame == 1) initShipMission();
        if (cerrSwitch && cerrBerth) {
            cerrBerthFun();
        }
        checkBerthBanned();
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