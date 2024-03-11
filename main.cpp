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
void getMissionPlus(int shipId);

int main() {
//    PathAlgorithm = 1;
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
    if (PathAlgorithm == 0) robotPath[robId] = getPath1(robId, goodsToGet.position);
    else if (PathAlgorithm == 1) robotPath[robId] = getPathbyAStar(robId, goodsToGet.position);
    else robotPath[robId] = getPath1(robId, goodsToGet.position);
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

Path getPathbyAStar(int robID, Point target) {
    int fxx[4] = {0, 0, 1, -1};
    int fyy[4] = {1, -1, 0, 0};
    vector<Point> points;
    struct node {
        Point position, target;
        int step;
        bool operator < (const node &b) const {
            return ((abs(position.x - target.x) + abs(position.y - target.y) + step) > (b.step + abs(b.position.x - b.target.x) + abs(b.position.y - target.y) ));
        }
    };
    priority_queue<node> q;
    q.push((node){robot[robID].position, target, 0});
    int b[300][300], dirc[300][300];
    char thismap[300][300];
    for (int i = 0; i < 210; i++)
        for (int j = 0; j < 210; j++) {
            b[i][j] = dirc[i][j] = 0;
            thismap[i][j] = maze[i][j];
        }
    int flag = 0, length = 0;
    while (!q.empty()) {
        node tp = q.top();
        q.pop();
        /*
         * getRobotPathArea
         * */
        for (int i = 0; i < 4; i++) {
            int ex = tp.position.x + fxx[i], ey = tp.position.y + fyy[i];
            if (ex < 0 || ex >= 200 || ey < 0 || ey >= 200) continue;
            if (b[ex][ey]) continue;
            if (thismap[ex][ey] != PointState::BLOCK && thismap[ex][ey] != PointState::OCEAN) {
                q.push((node){(Point){ex, ey}, target, tp.step + 1});
                b[ex][ey] = 1;
                dirc[ex][ey] = i;
                if (ex == target.x && ey == target.y) {
                    flag = 1;
                    break;
                }
            }
        }
        /*
         * returnMapArea
         * */
        if (flag == 1) break;
    }
    if (flag == 0) {
        return Path();
    }
    int nx = target.x, ny = target.y;
    stack<Point> repath;
    while (nx != robot[robID].position.x || ny != robot[robID].position.y) {
        repath.push((Point){nx, ny});
        int lastx = nx, lasty = ny;
        nx -= fxx[dirc[lastx][lasty]];
        ny -= fyy[dirc[lastx][lasty]];
        length++;
    }
    repath.push((Point){nx, ny});
    length++;
    while (!repath.empty()) {
        points.push_back(repath.top());
        repath.pop();
    }
    return Path(points, length);
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

void getMissionPlus(int shipId){
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
            visitGoods[targetBerth1] = 1;
        }
        else{
            ship[shipId].setMission(ShipMission(targetBerth1,cap1));
            ship[shipId].setMission(ShipMission(targetBerth2,cap2));
            berth[targetBerth1].visitGoods += cap1;
            berth[targetBerth2].visitGoods += cap2;
        }
    }
}
