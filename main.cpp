#include "Map.h"

int robotHome[12];
int berthRobot[12];
bool visitGoods[200007];
struct Operation{
    Goods targetGoods;
    int targetBerthId;
    double totalDistance;
    double efficiency;
    bool operator<(const Operation &x) const{
        return efficiency < x.efficiency;
    }
};
priority_queue<Operation>operation[12];

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
Path getPath1(int robId, Point target);
Path getPathbyAStar(int robId, Point target);
void robotSetMission(int robId, Goods goodsToGet, int targetBerthId);
void calcEfficiency(int start);
void getMission(int shipId);

int main() {
    freopen("out.txt", "w", stderr);
    Map::init();
    for(int i = 0; i <= 9; i++) Map::pretreatPath(berth[i]);
    Map::calcDistanceBetweenBerth();
    allocateHome();
//    for (int i = 0; i <= 9; i++) {
//        cerr << i << "'s home:" << robotHome[i] << '\n';
//    }
    while (frame < 15000){
//        cerr << "LKP AK IOI" << "\n";
        Map::update();
//        cerr << "frame=" << frame << '\n';
        for(int i = 0; i <= 4; i++) if(ship[i].isFree())getMission(i);
        for(int i = 0; i <= 9; i++) calcEfficiency(i);
        while(newGoods.size()) newGoods.pop_back();
//        cerr << "frameStep1:" << frame << '\n';
        for(int i = 0; i <= 9; i++){
            if(robot[i].getState() == RobotState::FREE){
                while(operation[robotHome[i]].size()){
                    int dis = operation[robotHome[i]].top().totalDistance / 2;
                    int time = operation[robotHome[i]].top().targetGoods.time;
                    if((frame + dis + 100 < time + 1000)
                        && (visitGoods[operation[robotHome[i]].top().targetGoods.id] == 0)) {
                        break;
                    }
                    operation[robotHome[i]].pop();
                }
                if(operation[robotHome[i]].empty())continue;
                robotSetMission(i, operation[robotHome[i]].top().targetGoods, operation[robotHome[i]].top().targetBerthId);
                visitGoods[operation[robotHome[i]].top().targetGoods.id] = 1;
                operation[robotHome[i]].pop();
            }
        }
        cout << "OK" << endl;
        cout.flush();
    }
    cerr << "跳帧数量" << framesum << "\n";
    return 0;
}

void robotSetMission(int robId, Goods goodsToGet, int targetBerthId) {
//    cerr << "robotSetMission tar:" << targetBerthId << '\n';
    robot[robId].setMission(goodsToGet, targetBerthId);
    robotPath[robId] = getPathbyAStar(robId, goodsToGet.position);
}

void allocateHome(){
    for(int i = 0; i <= 9; i++)robotHome[i] = berthRobot[i] = -1;
    int cnt = 0;
    for(int i = 0; i <= 9; i++){
        for(int j = 0; j <= 9; j++){
            to_berth_distance[++cnt].robotId = i;
            to_berth_distance[cnt].berthId = j;
            //to_berth_distance[cnt].distance = Path(robot[i].position, berth[j].position, -1).length;
            to_berth_distance[cnt].distance = Map::getLength(j,robot[i].position);
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
    cout.flush();
}

void calcEfficiency(int start){
    for(int i = 0; i < newGoods.size(); i++) {
        double dis = Map::getLength(start, newGoods[i].position) * 2;
        double efficiency = 1.0 * newGoods[i].value / (dis * 2.0);
        operation[start].push((Operation){newGoods[i], start, dis * 2, efficiency});
    }
}

void getMission(int shipId){
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
