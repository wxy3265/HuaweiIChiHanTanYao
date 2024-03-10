#include "Map.h"

int robotHome[12];
int berthRobot[12];
bool visitGoods[200007];
struct Operation{
    Goods targetGoods;
    int targetBerthId;
    int totalDistance;
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
void robotSetMission(int robId, Goods goodsToGet, int targetBerthId);
void calcEfficiency(int start);
void getMission(int shipId);

int main() {
    freopen("out.txt", "w", stderr);
    Map::init();
    for(int i = 0; i <= 9; i++) Map::pretreatPath(berth[i]);
    Map::calcDistanceBetweenBerth();
    allocateHome();
    while (frame < 15000){
        Map::update();
        for(int i = 0; i <= 4; i++)
            if(ship[i].isFree())getMission(i);
        for(int i = 0; i <= 9; i++)
            calcEfficiency(i);
        while(newGoods.size())newGoods.pop_back();
        for(int i = 0; i <= 9; i++){
//            if (i != 5 && i != 0 && i != 7 && i != 9) continue;
//            cerr << "berth:" << operation[robotHome[i]].top().targetBerthId << '\n';
            if (robotCrushed[i]) {
                if (robot[i].getMission() == RobotState::MISSION_GET) {
                    robotPath[i] = getPath1(i, robot[i].getGoodsToGet().position);
//                    robotCrushed[i] = false;
                } else if (robot[i].getMission() == RobotState::MISSION_PULL) {
                    robotPath[i] = getPath1(i, Point(berth[robotHome[i]].position.x + 3, berth[robotHome[i]].position.y + 3));
//                    robotCrushed[i] = false;
                }
            }
            if (robotGetGoods[i]) {
                robotPath[i] = getPath1(i, Point(berth[robotHome[i]].position.x + 3, berth[robotHome[i]].position.y + 3));
            }
            if(robot[i].getState() == RobotState::FREE){
                while(operation[robotHome[i]].size()){
                    int dis = operation[robotHome[i]].top().totalDistance / 2;
                    int time = operation[robotHome[i]].top().targetGoods.time;
                    if((frame + dis + 40 < time + 1000)
                        && (visitGoods[operation[robotHome[i]].top().targetGoods.id] == 0)) {
                        break;
                    }
                    operation[robotHome[i]].pop();
                }
                if(operation[robotHome[i]].empty())continue;
                robotSetMission(i, operation[robotHome[i]].top().targetGoods, operation[robotHome[i]].top().targetBerthId);
//                while (true) cerr << "FREE";
                visitGoods[operation[robotHome[i]].top().targetGoods.id] = 1;
                operation[robotHome[i]].pop();
            }
        }
        cout << "OK" << endl;
        cout.flush();
    }
//    cerr << "跳帧数量" << framesum << "\n";
    return 0;
}

void robotSetMission(int robId, Goods goodsToGet, int targetBerthId) {
//    cerr << "robotSetMission tar:" << targetBerthId << '\n';
    robot[robId].setMission(goodsToGet, targetBerthId);
    robotPath[robId] = getPath1(robId, goodsToGet.position);
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
        if(!Map::isOpen(to_berth_distance[i].berthId))continue;
        if(robotHome[to_berth_distance[i].robotId] != -1)continue;
        if(berthRobot[to_berth_distance[i].berthId] != -1)continue;
        if(to_berth_distance[i].distance > 50000)continue;
        robotHome[to_berth_distance[i].robotId] = to_berth_distance[i].berthId;
        berthRobot[to_berth_distance[i].berthId] = to_berth_distance[i].robotId;
    }
    for(int i = 1; i <= cnt; i++){
        if(!Map::isOpen(to_berth_distance[i].berthId))continue;
        if(robotHome[to_berth_distance[i].robotId] != -1)continue;
        if(to_berth_distance[i].distance > 50000)continue;
        robotHome[to_berth_distance[i].robotId] = to_berth_distance[i].berthId;
    }
    cout.flush();
}

void makeMap(vector<Point> points) {
//    freopen("Pathmap.txt", "w", stderr);
    char pathinMap[300][300];
    for (int i = 0; i < 200; i++)
        for (int j = 0; j < 200; j++)
            pathinMap[i][j] = maze[i][j];
    for (int i = 0; i < points.size(); i++)
        pathinMap[points[i].x][points[i].y] = '~';
    for (int i = 0; i < 200; i++) {
        for (int j = 0; j < 200; j++)
            cerr << pathinMap[i][j];
        cerr << "\n";
    }
//    fclose(stderr);
}

Path getPath1(int robId, Point target) {
    int fxx[4] = {0, 0, 1, -1};
    int fyy[4] = {1, -1, 0, 0};
    int step[300][300], stepnum[300][300], ffind[300][300], flag = 0;
    char thismap[300][300];
    for (int i = 0; i < 210; i++) {
        for (int j = 0; j < 210; j++) {
            step[i][j] = stepnum[i][j] = ffind[i][j] = 0;
            thismap[i][j] = maze[i][j];
        }
    }
    vector<Point> points;
    queue<Point> q;
    q.push(robot[robId].position);
//    cerr << "getPath1:" << robId << '\n';
//    int ssss = 0;
    while (!q.empty()) {
//        ssss++;
        Point fr = q.front();
        q.pop();
        int nextframe = step[fr.x][fr.y] + 1;
        for (int i = 0; i < 10; i++) {
            if (i != robId) {
                if (robotPath[i].length > 50000) continue;
//                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
//                if (robotThisPoint1 != Point(-1, -1)) {
//                    thismap[robotThisPoint1.x][robotThisPoint1.y] = PointState::BLOCK;
//                }

                Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
                Point robotThisPoint2 = robotPath[i].getPointbyTime(nextframe + 1);


//                if(robotCrushed[i] && robotCrushed[robId]) {
////                    cerr << "Avoid Crushed!" << i << ' ' << robId << '\n';
//                    thismap[robotThisPoint0.x][robotThisPoint0.y] = PointState::BLOCK;
//                }
                if (robotThisPoint0 != Point(-1, -1)) {
                    thismap[robotThisPoint0.x][robotThisPoint0.y] = PointState::BLOCK;
                }
                if (robotThisPoint1 != Point(-1, -1)) {
                    thismap[robotThisPoint1.x][robotThisPoint1.y] = PointState::BLOCK;
                }
                if (robotThisPoint2 != Point(-1, -1)) {
                    thismap[robotThisPoint2.x][robotThisPoint2.y] = PointState::BLOCK;
                }

//                cerr << ssss << " " << nextframe << " " << i << " " << robId << " " << robotThisPoint.x << " " << robotThisPoint.y << "\n";
            }
        }
        for (int i = 0; i < 4; i++) {
            int ex = fr.x + fxx[i], ey = fr.y + fyy[i];
            if (ex < 0 || ex > 199 || ey < 0 || ey > 199) continue;
            if (ffind[ex][ey]) continue;
            if (thismap[ex][ey] != PointState::OCEAN && thismap[ex][ey] != PointState::BLOCK) {
                q.push((Point){ex, ey});
                ffind[ex][ey] = 1;
                step[ex][ey] = step[fr.x][fr.y] + 1;
                stepnum[ex][ey] = i;
                if (ex == target.x && ey == target.y) {
                    flag = 1;
                    break;
                }
            }
        }
        for (int i = 0; i < 10; i++) {
            if (i != robId) {
                if (robotPath[i].length > 50000) continue;
//                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
//                if (robotThisPoint1.x != -1 && robotThisPoint1.y != -1)
//                    thismap[robotThisPoint1.x][robotThisPoint1.y] = maze[robotThisPoint1.x][robotThisPoint1.y];

                Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
                Point robotThisPoint2 = robotPath[i].getPointbyTime(nextframe + 1);
//                if(robotCrushed[i] && robotCrushed[robId]) {
////                    cerr << "Avoid Crushed!" << i << ' ' << robId << '\n';
//                    thismap[robotThisPoint0.x][robotThisPoint0.y] = maze[robotThisPoint0.x][robotThisPoint0.y];
//                }
                if (robotThisPoint0.x != -1 && robotThisPoint0.y != -1)
                    thismap[robotThisPoint0.x][robotThisPoint0.y] = maze[robotThisPoint0.x][robotThisPoint0.y];
                if (robotThisPoint1.x != -1 && robotThisPoint1.y != -1)
                    thismap[robotThisPoint1.x][robotThisPoint1.y] = maze[robotThisPoint1.x][robotThisPoint1.y];
                if (robotThisPoint2.x != -1 && robotThisPoint2.y != -1)
                    thismap[robotThisPoint2.x][robotThisPoint2.y] = maze[robotThisPoint2.x][robotThisPoint2.y];

            }
        }
    }
    if (flag == 0) {

        return Path();
    }
    int nx = target.x, ny = target.y, length = 0;
    stack<Point> repath;
    while (nx != robot[robId].position.x || ny != robot[robId].position.y) {
        repath.push((Point){nx, ny});
        int lastx = nx, lasty = ny;
        nx -= fx[stepnum[lastx][lasty]];
        ny -= fy[stepnum[lastx][lasty]];
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

Path getPathbyAStar(int robID, Point target) {
    int fxx[4] = {0, 0, 1, -1};
    int fyy[4] = {1, -1, 0, 0};
    vector<Point> points;
    struct node {
        Point position, target;
        int step;
        bool operator < (const node &b) const {
            return ((abs(position.x - target.x) + abs(position.y - target.y)) > (abs(b.position.x - b.target.x) + abs(b.position.y - target.y)));
        }
    };
    priority_queue<node> q;
    q.push((node){robot[robID].position, target, 0});
    int b[300][300], dirc[300][300];
    char thismap[300][300];
    for (int i = 0; i < 200; i++)
        for (int j = 0; j < 200; j++) {
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
        int dis = Map::getLength(start, newGoods[i].position);
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
        ship[shipId].setMission(ShipMission(targetBerth, 12));
        visitBerth[targetBerth] = true;
    }
}
