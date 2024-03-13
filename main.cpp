#include "Map.h"

int robotHome[10];
int berthRobot[10];
bool visitGoods[200007];
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

bool cmp(Distance_to_berth x, Distance_to_berth y){
    return x.distance < y.distance;
}

void allocateHome();
void makeMap(vector<Point> points);
void robotSetMission(int robId, Goods goodsToGet, int targetBerthId);
void calcEfficiency(int startBerthId);
void calcEfficiencyMax(int startBerthId);
void calcEfficiencyPlus(int robId);
void calcEfficiencyUltra(int startBerthId);
void shipGetMission(int shipId);
void shipGetMissionSE(int shipId);
void shipGetMissionSEPlus(int shipId);
void shipGetMissionMiniPlus(int shipId);
void shipGetMissionMini(int shipId);
void shipGetMissionPlus(int shipId);
void robotGetMissionFromOperation(int robId);
void robotGetMissionFromOperationPlus(int robId);

int main() {
//    PathAlgorithm = 1;
    freopen("out.txt", "w", stderr);
    Map::init();
    Map::calcDistanceBetweenBerth();
    for(int i = 0; i <= 9; i++) Map::pretreatPath(i), robotFirstMission[i] = true;
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
        for(int i = 0; i <= 4; i++) if(ship[i].isFree()) shipGetMissionMiniPlus(i);
        for(int i = 0; i <= 9; i++) calcEfficiencyMax(i);
        while(!newGoods.empty()) newGoods.pop_back();
        for(int i = 0; i <= 9; i++)
            if(robot[i].getState() == RobotState::FREE)
                robotGetMissionFromOperation(i);
        cout << "OK" << endl;
        cout.flush();
    }
    cerr << "跳帧数量" << framesum << "\n";
    return 0;
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
           && operation[robotHome[robId]].top().goodsDistance <= 800000) {
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
    while(!operationPlus[robId].empty()){
        double dis = operationPlus[robId].top().totalDistance;
        int time = operationPlus[robId].top().targetGoods.time;
        if((frame + dis + 100 < time + 1000)
           && !visitGoods[operationPlus[robId].top().targetGoods.id]) {
            break;
        }
        operationPlus[robId].pop();
    }
    if(operationPlus[robId].empty()) return;
    robotSetMission(robId, operationPlus[robId].top().targetGoods, operationPlus[robId].top().targetBerthId);
    cerr << "rob[" << robId << "] getGoods " << operationPlus[robId].top().targetGoods.id << '\n';
    visitGoods[operationPlus[robId].top().targetGoods.id] = true;
    operationPlus[robId].pop();
}
void robotSetMission(int robId, Goods goodsToGet, int targetBerthId) {
//    cerr << "robotSetMission tar:" << targetBerthId << '\n';
    robot[robId].setMission(goodsToGet, targetBerthId);
    robotFirstMission[robId] = false;
    if (PathAlgorithm == 0) robotPath[robId] = getPath1(robId, goodsToGet.position);
    else if (PathAlgorithm == 1) robotPath[robId] = getPathbyAStar(robId, goodsToGet.position);
    else robotPath[robId] = getPath1(robId, goodsToGet.position);
//    robotHome[robId] = targetBerthId;
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
void calcEfficiency(int startBerthId) {
    for(auto & newGood : newGoods) {
        int nearBerthId = Map::getNearBerthId(newGood.position);
        double pathLength = Map::getLengthFromBerthToPoint(startBerthId, newGood.position) * 2;
        double efficiency = 1.0 * newGood.value / pathLength;
//        if (newGood.value > 150)
        operation[startBerthId].push((Operation){newGood, startBerthId, pathLength, pathLength / 2.0, efficiency});
    }
}
void calcEfficiencyMax(int startBerthId) {
    double deltaLength = 0.56;
    for(auto & newGood : newGoods) {
        int nearBerthId = Map::getNearBerthId(newGood.position);
        //if(nearBerthId != startBerthId)continue;
        double pathLength = Map::getLengthFromBerthToPoint(nearBerthId, newGood.position) * 2;
        double pathLength1 = Map::getLengthFromBerthToPoint(startBerthId, newGood.position) * 2;
        double efficiency = 1.0 * newGood.value / pathLength;
        double efficiency1 = 1.0 * (newGood.value - (pathLength1 - pathLength) * deltaLength) / pathLength1;
//        if (newGood.value > 150)
        operation[nearBerthId].push((Operation){newGood, nearBerthId, pathLength, pathLength / 2.0, efficiency});
        if(nearBerthId != startBerthId)operation[startBerthId].push((Operation){newGood,startBerthId,pathLength1,pathLength1 / 2.0, efficiency1});
    }
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
        int nearBerthId = Map::getNearBerthId(newGood.position);
        int nowBerthId;
        if (robotFirstMission[robId]) nowBerthId = robotHome[robId];
        else nowBerthId = robot[robId].getTargetId();
        double pathLength = Map::getLengthFromBerthToPoint(nowBerthId, newGood.position) +
                            Map::getLengthFromBerthToPoint(nearBerthId, newGood.position) +
                            berth[nearBerthId].distance * 0;
        double goodsDistance = Map::getLengthFromBerthToPoint(nowBerthId, newGood.position);
        double efficiency = 2 * newGood.value / pathLength;
//        if (newGood.value > 150)
        operationPlus[robId].push((Operation){newGood, nearBerthId, pathLength, goodsDistance, efficiency});
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
int setedShipMissionTimes[10];
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
            //berth[targetBerth1].visitGoods += cap1;
            visitGoods[targetBerth1] = true;
        }
        else{
            ship[shipId].setMission(ShipMission(targetBerth1,-1));
            ship[shipId].setMission(ShipMission(targetBerth2,-1));
            berth[targetBerth1].visitGoods += cap1;
            berth[targetBerth2].visitGoods += cap2;
        }
    }
}
