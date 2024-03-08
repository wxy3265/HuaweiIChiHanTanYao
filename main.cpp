//#include "tanyao.h"
#include "Map.h"

int robotHome[12];
int berthRobot[12];
bool visitGoods[200007];
struct Operation{
    Goods targetGoods;
    Berth targetBerth;
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

void allocateHome(){
    for(int i = 0; i <= 9; i++)robotHome[i] = berthRobot[i] = -1;
    int cnt = 0;
    for(int i = 0; i <= 9; i++){
        for(int j = 0; j <= 9; j++){
            to_berth_distance[++cnt].robotId = i;
            to_berth_distance[cnt].berthId = j;
            to_berth_distance[cnt].distance = Path(robot[i].position, berth[j].position, 0).length;
        }
    }
    sort(to_berth_distance + 1,to_berth_distance + cnt + 1,cmp);

    for(int i = 1; i <= cnt; i++){
        if(robotHome[to_berth_distance[i].robotId] != -1)continue;
        if(berthRobot[to_berth_distance[i].berthId] != -1)continue;
        robotHome[to_berth_distance[i].robotId] = to_berth_distance[i].berthId;
        berthRobot[to_berth_distance[i].berthId] = to_berth_distance[i].robotId;
    }

}

void makeMap(vector<Point> points) {
    freopen("Pathmap.txt", "w", stdout);
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
    fclose(stdout);
}

void getPath1(int id, Point end) {
    int fxx[4] = {0, 0, 1, -1};
    int fyy[4] = {1, -1, 0, 0};
    int step[300][300], stepnum[300][300], ffind[300][300], flag = 0;
    char thismap[300][300];
    for (int i = 0; i < 210; i++)
        for (int j = 0; j < 210; j++) {
            step[i][j] = stepnum[i][j] = ffind[i][j] = 0;
            thismap[i][j] = maze[i][j];
        }
    vector<Point> points;
    queue<Point> q;
    q.push(robot[id].position);
    while (!q.empty()) {
        Point fr = q.front();
        q.pop();
        int nextframe = step[fr.x][fr.y] + 1;
        for (int i = 0; i < 10; i++) {
            if (i != id) {
                if (robot[i].path.length = 10000000) continue;
                Point robotThisPoint = robot[i].path.getPointbyTime(nextframe);
                thismap[robotThisPoint.x][robotThisPoint.y] = PointState::BLOCK;
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
                if (ex == end.x && ey == end.y) {
                    flag = 1;
                    break;
                }
            }
        }
        for (int i = 0; i < 10; i++)
            if (i != id) {
                if (robot[i].path.length = 10000000) continue;
                Point robotThisPoint = robot[i].path.getPointbyTime(nextframe);
                thismap[robotThisPoint.x][robotThisPoint.y] = maze[robotThisPoint.x][robotThisPoint.y];
            }
    }
    if (flag == 0) {

        return;
    }
    int nx = end.x, ny = end.y, length = 0;
    stack<Point> repath;
    while (nx != robot[id].position.x || ny != robot[id].position.y) {
        repath.push((Point){nx, ny});
        int lastx = nx, lasty = ny;
        nx -= fx[stepnum[lastx][lasty]];
        ny -= fy[stepnum[lastx][lasty]];
        length++;
    }
    repath.push((Point){nx, ny});
    length++;
//    cerr << "start:" << start.x << ',' << start.y << ' ' << "end:" << end.x << ',' << end.y << '\n';
//    cerr << "id:" << id << "path:";
    while (!repath.empty()) {
        points.push_back(repath.top());
//        cerr << repath.top().x << ',' << repath.top().y << ' ';
        repath.pop();
    }
//    cerr << '\n';
//        while(!id && start.x == 2) ;
    robot[id].path.setPath(points, length);
    makeMap(points);

}

void calcEfficiency(int start){
    for(int i = 0; i < newGoods.size(); i++) {
        int dis = Path(newGoods[i].position, berth[start].position, 0).length;
        double efficiency = 1.0 * newGoods[i].value / (dis * 2.0);
        operation[start].push((Operation){newGoods[i], berth[start], dis * 2, efficiency});
    }
}

int main() {
    freopen("out.txt", "w", stderr);
    Map::init();
    allocateHome();
    while (frame < 15000){
        Map::update();
        for(int i = 0; i <= 9; i++)
            calcEfficiency(i);
        while(newGoods.size())newGoods.pop_back();
        for(int i = 0; i <= 9; i++){
            if(robot[i].getState() == RobotState::FREE){
                while(operation[robotHome[i]].size()){
                    int dis = operation[robotHome[i]].top().totalDistance / 2;
                    int time = operation[robotHome[i]].top().targetGoods.time;
                    if((frame + dis < time + 1000) && (visitGoods[operation[robotHome[i]].top().targetGoods.id] == 0) && (dis > 50000))break;
                    operation[robotHome[i]].pop();
                }
                if(operation[robotHome[i]].empty())continue;
                robot[i].setMission(operation[robotHome[i]].top().targetGoods, operation[robotHome[i]].top().targetBerth);
                visitGoods[operation[robotHome[i]].top().targetGoods.id] = 1;
                operation[robotHome[i]].pop();
            }
        }
        cout << "OK" << endl;
        cout.flush();
    }
    return 0;
}
