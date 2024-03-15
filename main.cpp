#include "Map.h"

int robotHome[10];
int berthRobot[10];
int setedShipMissionTimes[10];
bool robotFirstMission[10];
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
}to_berth_distance[107];

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
void makeMap(vector<Point> points);
void robotSetMission(int robId, Goods goodsToGet, int targetBerthId);
void calcEfficiency(int startBerthId);
void calcEfficiencyLarge(int startBerthId);
void calcEfficiencyMax(int startBerthId);
void calcEfficiencyPlus(int robId);
void calcEfficiencyUltra(int startBerthId);
void shipGetMission(int shipId);
void shipGetMissionSE(int shipId);
void shipGetMissionSEPlus(int shipId);
void shipGetMissionMiniPlus(int shipId);
void shipGetMissionMini(int shipId);
void shipGetMissionPlus(int shipId);
void robotGetMission(int robId);
void robotGetMissionFromOperation(int robId);
void robotGetMissionFromOperationPlus(int robId);

int main() {
//    PathAlgorithm = 1;
    freopen("out.txt", "w", stderr);
    Map::init();
    Map::calcDistanceBetweenBerth();
    for(int i = 0; i <= 9; i++) Map::pretreatPathToBerth(i), robotFirstMission[i] = true;
    for(int i = 0; i <= 9; i++) Map::pretreatPathToStart(i);
    for (int i = 0; i < 200; i++) {
        for (int j = 0; j < 200; j++) {
            cerr << Map::getNearBerthId(Point(i, j));
        }
        cerr << '\n';
    }
    allocateHome();
//    for (int i = 0; i < 10; i++) cerr << i << "'s home:" << robotHome[i] << '\n';
    cout.flush();
    while (frame < 15000){
        Map::update();
//        if (cerrFrame)
        cerr << "frame=" << frame << '\n';
        int totBerthValue = 0;
        for (int i = 0; i < 10; i++) {
            cerr << "berth:[" << i << "] value:<" << berth[i].getTotalValue()
                 << "> numbers:" << berth[i].getGoodsNum()
                 << " distance:" << berth[i].distance
                 << '\n';
            totBerthValue += berth[i].getTotalValue();
        }
        cerr << "totBerthValue: <" << totBerthValue << ">\n";
        for (int i = 0; i <= 9; i++) {
            if (berthBanned[i]) {
                berthBanned[i] = false;
                for (int j = 0; j <= 9; j++) {
                    if (robotHome[j] == i) {
                        reallocateHome(j);
                        cerr << "reallocateHome:[" << j << "]\n";
                    }
                }
            }
        }
        for (int i = 0; i <= 4; i++) if(ship[i].isFree()) shipGetMissionPlus(i);
//        for (int i = 0; i <= 9; i++) calcEfficiencyMax(i);
        while (!newGoods.empty()) newGoods.pop_back();
        if (true) {
            for (int i = 0; i <= 9; i++)
                if (robot[i].getState() == RobotState::FREE)
                    robotGetMission(i);
        } else {
            for (int i = 0; i <= 9; i++)
                if (robot[i].getState() == RobotState::FREE)
                    robotGetMissionFromOperation(i);
        }
        cout << "OK" << endl;
        cout.flush();
    }
    cerr << "跳帧数量" << framesum << "\n";
    return 0;
}

void allocateHome(){
    for(int i = 0; i <= 9; i++) robotHome[i] = berthRobot[i] = -1;
    int cnt = 0;
    for(int i = 0; i <= 9; i++){
        for(int j = 0; j <= 9; j++){
            to_berth_distance[++cnt].robotId = i;
            to_berth_distance[cnt].berthId = j;
            //to_berth_distance[cnt].distance = Path(robot[i].position, berth[j].position, -1).length;
            to_berth_distance[cnt].distance = Map::getLengthFromBerthToPoint(j, robot[i].position);
        }
    }
    sort(to_berth_distance + 1,to_berth_distance + cnt + 1,cmp);

    for(int i = 1; i <= cnt; i++){
        if(to_berth_distance[i].distance > 100000)continue;
        if(!Map::isOpen(to_berth_distance[i].berthId))continue;
        if(robotHome[to_berth_distance[i].robotId] != -1)continue;
        if(berthRobot[to_berth_distance[i].berthId] != -1)continue;
        robotHome[to_berth_distance[i].robotId] = to_berth_distance[i].berthId;
        berthRobot[to_berth_distance[i].berthId] = to_berth_distance[i].robotId;
    }
    for(int i = 1; i <= cnt; i++){
        if(to_berth_distance[i].distance > 100000)continue;
        if(!Map::isOpen(to_berth_distance[i].berthId))continue;
        if(robotHome[to_berth_distance[i].robotId] != -1)continue;
        robotHome[to_berth_distance[i].robotId] = to_berth_distance[i].berthId;
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
//        cerr << "OK\n";
        double dis = operation[robotHome[robId]].top().goodsDistance;
        int time = operation[robotHome[robId]].top().targetGoods.time;
        if((frame + dis + 10 < time + 1000)
           && !visitGoods[operation[robotHome[robId]].top().targetGoods.id]
           && operation[robotHome[robId]].top().goodsDistance <= 100) {
            break;
        }
        operation[robotHome[robId]].pop();
    }
    if(operation[robotHome[robId]].empty()) return;
    robotSetMission(robId, operation[robotHome[robId]].top().targetGoods, operation[robotHome[robId]].top().targetBerthId);
    visitGoods[operation[robotHome[robId]].top().targetGoods.id] = true;
    operation[robotHome[robId]].pop();
}
void robotGetMissionFromOperationPlus(int robId) {
//    cerr << "robGetPlusID:[" << robId << "]\n";
    int nowBerthId;
    if (robotFirstMission[robId]) nowBerthId = robotHome[robId];
    else nowBerthId = robot[robId].getTargetId();
    if (nowBerthId == -1) return;
    while(!operation[nowBerthId].empty()){
        double dis = operation[nowBerthId].top().goodsDistance;
        int time = operation[nowBerthId].top().targetGoods.time;
        if((frame + dis + 10 < time + 1000)
           && !visitGoods[operation[nowBerthId].top().targetGoods.id]
              && operation[nowBerthId].top().goodsDistance <= 1500000) {
            break;
        }
        operation[nowBerthId].pop();
    }
    if(operation[nowBerthId].empty()) return;
    robotSetMission(robId, operation[nowBerthId].top().targetGoods, operation[nowBerthId].top().targetBerthId);
    cerr << "rob[" << robId << "] getGoods " << operation[nowBerthId].top().targetGoods.id << '\n';
    visitGoods[operation[nowBerthId].top().targetGoods.id] = true;
    operation[nowBerthId].pop();
}
void robotSetMission(int robId, Goods goodsToGet, int targetBerthId) {
//    cerr << "robotSetMission tar:" << targetBerthId << '\n';
    visitGoods[goodsToGet.id] = true;
    robot[robId].setMission(goodsToGet, targetBerthId);
    robotFirstMission[robId] = false;
//    robotHome[robId] = targetBerthId;
}
void robotGetMission(int robId) {
    if (robotHome[robId] == -1) return;
    int nowBerthId;
    if (robotFirstMission[robId]) nowBerthId = -2;
    else nowBerthId = robot[robId].getTargetId();
    if (nowBerthId == -1) return;
    int n = goodsOnMap.size();
    priority_queue <GoodsMission> goodsMission;
    cerr << "goodsOnMapSize:" << goodsOnMap.size() << '\n';
    for (int i = 0; i < n; i++) {
        Goods goods = goodsOnMap[i];
//        if (goods.value < 100) continue;
        GoodsMission goodsMissionNow;
        int nearBerthId = Map::getNearBerthId(goods.position);
//        int distance = Map::getLengthFromBerthToPoint(nearBerthId, goods.position);
        int distance = 0;
        if (nowBerthId == -2) distance += Map::getLengthFromStartToPoint(robId, goods.position);
        else distance += Map::getLengthFromBerthToPoint(nowBerthId, goods.position);
        if (frame + distance + 10 >= goods.time + 1000) continue;
        goodsMissionNow.goods = goods;
        goodsMissionNow.key = (double) goods.value / (double)distance;
        goodsMission.push(goodsMissionNow);
    }
    if (goodsMission.empty()) return;
    GoodsMission targetMission = goodsMission.top();
    int targetBerthId = Map::getNearBerthId(targetMission.goods.position);
    robotSetMission(robId, targetMission.goods, targetBerthId);
}
void calcEfficiency(int startBerthId) {
    for(auto & newGood : newGoods) {
        int nearBerthId = Map::getNearBerthId(newGood.position);
        double pathLength = Map::getLengthFromBerthToPoint(startBerthId, newGood.position) * 2;
        double efficiency = 1.0 * newGood.value / pathLength;
//        if (newGood.value > 150)
        operation[startBerthId].push((Operation){newGood, startBerthId, pathLength, pathLength / 2.0, efficiency});
    }
}
void calcEfficiencyLarge(int startBerthId) {
    for(auto & newGood : newGoods) {
        if (newGood.value < 100) continue;
        int nearBerthId = Map::getNearBerthId(newGood.position);
        double pathLength = Map::getLengthFromBerthToPoint(startBerthId, newGood.position) * 2;
        double efficiency = 1.0 * 20000 / pathLength;
//        if (newGood.value > 150)
        operation[startBerthId].push((Operation){newGood, startBerthId, pathLength, pathLength / 2.0, efficiency});
    }
}
void checkOperation(int berthId) {
    cerr << "berthID:" <<berthId << "\n";
    priority_queue<Operation> q;
    while (!operation[berthId].empty()) {
        q.push(operation[berthId].top());
        cerr << "totaldistance:" <<operation[berthId].top().totalDistance << "\n";
        cerr << "efficiency:" << operation[berthId].top().efficiency << "\n";
        cerr << "targetGoods.value:" << operation[berthId].top().targetGoods.value << "\n";
        operation[berthId].pop();
    }
    while (!q.empty()) {
        operation[berthId].push(q.top());
        q.pop();
    }
    cerr << "\n";
}
void calcEfficiencyMax(int startBerthId) {
    double deltaLength = 0.56;
    double deltaTime = 0.5;
    for(auto & newGood : newGoods) {
        int nearBerthId = Map::getNearBerthId(newGood.position);
        //if(nearBerthId != startBerthId)continue;
        double pathLength = Map::getLengthFromBerthToPoint(nearBerthId, newGood.position) * 2;
        double pathLength1 = Map::getLengthFromBerthToPoint(startBerthId, newGood.position) * 2;
        double goodsTime = newGood.time;
        double efficiency = 100.0 * newGood.value / pathLength;
        double efficiency1 = 100.0 * (newGood.value - (pathLength1 - pathLength) * deltaLength)  / (pathLength1 + (goodsTime + 1000 - frame) * deltaTime);
//        if (newGood.value < 150) return;
        //operation[nearBerthId].push((Operation){newGood, nearBerthId, pathLength, pathLength / 2.0, efficiency});
        //if(nearBerthId != startBerthId)
//        if ((startBerthId == 6 || startBerthId == 8 || startBerthId == 4 || startBerthId == 7) && (newGood.position.x <= 100 || newGood.position.y <= 100)) continue;
        operation[startBerthId].push((Operation){newGood,startBerthId,pathLength1,pathLength1 / 2.0, efficiency1});
    }
//    checkOperation(startBerthId);
}
void calcEfficiencyUltra(int startBerthId) {
    for(auto & newGood : newGoods) {
        int nearBerthId = Map::getNearBerthId(newGood.position);
        if(nearBerthId != startBerthId) continue;
        double pathLength = Map::getLengthFromBerthToPoint(startBerthId, newGood.position) * 2;
        double efficiency = 1.0 * newGood.value / pathLength;
//        if (newGood.value > 150)
        operation[nearBerthId].push((Operation){newGood, nearBerthId, pathLength, pathLength / 2.0, efficiency});
    }
}
void calcEfficiencyPlus(int robId){
//    cerr << "begin";
    for(auto & newGood : newGoods) {
//        if (newGood.value < 150) continue;
        int nearBerthId = Map::getNearBerthId(newGood.position);
        int nowBerthId;
        if (robotFirstMission[robId]) nowBerthId = robotHome[robId];
        else nowBerthId = robot[robId].getTargetId();
        if (nowBerthId == -1) continue;
        double pathLength = Map::getLengthFromBerthToPoint(nowBerthId, newGood.position) +
                            Map::getLengthFromBerthToPoint(nearBerthId, newGood.position) +
                            berth[nearBerthId].distance * 0;
        double goodsDistance = Map::getLengthFromBerthToPoint(nowBerthId, newGood.position);
//        double efficiency = 1.0 * 200 / pathLength;
        double efficiency = (double) newGood.value / (double) pathLength;
//        if (newGood.value > 150)
        cerr << "nowBerthID:" << nowBerthId << '\n';
        operation[nowBerthId].push((Operation){newGood, nearBerthId, pathLength, goodsDistance, efficiency});
    }
//    cerr << "end\n";
}
void shipGetMission(int shipId){
    double efficiency = -1000000;
    int targetBerth = -1;
    for(int i = 0; i <= 9; i++){
        if(visitBerth[i])continue;
        int num = berth[i].getGoodsNum();
        int k = 0;
        while(k * capacity < num){
            k++;
            int minum = min(k * capacity, num);
            double eff = 1.0 * berth[i].getHeadGoodsValue(minum) / (berth[i].distance * 2.0 * k);
            if(eff > efficiency){
                efficiency = eff;
                targetBerth = i;
            }
        }
    }
    if(targetBerth != -1){
        ship[shipId].setMission(ShipMission(targetBerth, -1));
        visitBerth[targetBerth] = true;
    }
}
void shipGetMissionSE(int shipId) {
    ship[shipId].setMission((ShipMission){shipId * 2, -1});
    ship[shipId].setMission((ShipMission){shipId * 2 + 1, -1});
}
void shipGetMissionSEPlus(int shipId) {
    setedShipMissionTimes[shipId]++;
    if (setedShipMissionTimes[shipId] % 2 == 0) {
        ship[shipId].setMission((ShipMission){shipId * 2, min(setedShipMissionTimes[shipId] * 5, capacity)});
    }
    else {
        ship[shipId].setMission((ShipMission){shipId * 2 + 1, min(setedShipMissionTimes[shipId] * 5, capacity)});
    }
}
void shipGetMissionMini(int shipId) {
    for (int i = shipId * 2; i < 10; i++) {
        ship[shipId].setMission(ShipMission{i, -1});
    }
    for (int i = 0; i < shipId * 2; i++) {
        ship[shipId].setMission(ShipMission{i, -1});
    }
}
void shipGetMissionMiniPlus(int shipId) {
    double efficiency = -1000000;
    int targetBerth1 = -1,targetBerth2 = -1;
    int cap1 = 0,cap2 = 0;
    for(int i = 0; i <= 9; i++){
        int num = berth[i].getGoodsNum();
        int minum = min(num, capacity);
        double eff = 1.0 * minum / (berth[i].distance * 2.0);
        if(eff > efficiency){
            efficiency = eff;
            targetBerth1 = i;
            cap1 = minum;
        }
    }

    for (int i = 0; i <= 9; i++) {
        int num1 = berth[i].getGoodsNum();
        for(int j = 0; j <= 9; j++){
            if(i == j)continue;
            int num2 = berth[j].getGoodsNum();
            for (int cnt1 = 0; cnt1 <= min(num1, capacity); cnt1++) {
                int cnt2 = min(capacity - cnt1, num2);
                double eff = 1.0 * (cnt1 + cnt2) / (berth[i].distance + berth[j].distance + 500.0);
                if(eff > efficiency){
                    if(cnt1 == 0){
                        efficiency = eff;
                        targetBerth1 = i;
                        cap1 = cnt1;
                    }else if(cnt2 == 0){
                        efficiency = eff;
                        targetBerth1 = j;
                        cap1 = cnt2;
                    }else{
                        efficiency = eff;
                        targetBerth1 = i;
                        targetBerth2 = j;
                        cap1 = cnt1;
                        cap2 = cnt2;
                    }
                }
            }
        }
    }
    if(targetBerth1 != -1){
        if(targetBerth2 == -1){
            ship[shipId].setMission(ShipMission(targetBerth1,-1));
            berth[targetBerth1].visitGoods += cap1;
            //visitGoods[targetBerth1] = true;
        }
        else{
            ship[shipId].setMission(ShipMission(targetBerth1,-1));
            ship[shipId].setMission(ShipMission(targetBerth2,-1));
            berth[targetBerth1].visitGoods += cap1;
            berth[targetBerth2].visitGoods += cap2;
        }
    }
}
void shipGetMissionPlus(int shipId){
    double efficiency = -1000000;
    int targetBerth1 = -1,targetBerth2 = -1;
    int cap1 = 0,cap2 = 0;
    for(int i = 0; i <= 9; i++){
        int num = berth[i].getGoodsNum();
        int minum = min(num, capacity);
        double eff = 1.0 * berth [i].getHeadGoodsValue(minum) / (berth[i].distance * 2.0);
        if(eff > efficiency){
            efficiency = eff;
            targetBerth1 = i;
            cap1 = minum;
        }
    }

    for (int i = 0; i <= 9; i++) {
        int num1 = berth[i].getGoodsNum();
        for(int j = 0; j <= 9; j++){
            if(i == j)continue;
            int num2 = berth[j].getGoodsNum();
            for (int cnt1 = 0; cnt1 <= min(num1, capacity); cnt1++) {
                int cnt2 = min(capacity - cnt1, num2);
                double eff = 1.0 * (berth[i].getHeadGoodsValue(cnt1) + berth[j].getHeadGoodsValue(cnt2)) / (berth[i].distance + berth[j].distance + 500.0);
                if(eff > efficiency){
                    if(cnt1 == 0){
                        efficiency = eff;
                        targetBerth1 = i;
                        cap1 = cnt1;
                    }else if(cnt2 == 0){
                        efficiency = eff;
                        targetBerth1 = j;
                        cap1 = cnt2;
                    }else{
                        efficiency = eff;
                        targetBerth1 = i;
                        targetBerth2 = j;
                        cap1 = cnt1;
                        cap2 = cnt2;
                    }
                }
            }
        }
    }
    if(targetBerth1 != -1){
        if(targetBerth2 == -1){
            ship[shipId].setMission(ShipMission(targetBerth1,-1));
            berth[targetBerth1].visitGoods += cap1;
            //visitGoods[targetBerth1] = true;
        }
        else{
            ship[shipId].setMission(ShipMission(targetBerth1,-1));
            ship[shipId].setMission(ShipMission(targetBerth2,-1));
            berth[targetBerth1].visitGoods += cap1;
            berth[targetBerth2].visitGoods += cap2;
        }
    }
}
