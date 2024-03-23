//
// Created by wxy3265 on 2024/3/7.
//

#include "Robot.h"

bool nowFrameUse = false;
void makeMap(vector<Point> points);
int Robot::getState() {return state;}
void Robot::setMission(Goods _goodsToGet) {
    goodsToGet = _goodsToGet;
    mission = RobotState::MISSION_GET;
    state = RobotState::MISSION_MOVE;
    nextPoint = position;
}
int totGetValue = 0;
int totGiveUp = 0;
//更新机器人状态
void Robot::updateState(Point _position, bool _enable, bool _carrying) {
    lastPosition = position;
    position = _position;
    enable = _enable;
    carrying = _carrying;
    if (cerrRobot && cerrSwitch) {
        cerr << "robot[" << id << "] target:[" << targetId << "] " << berth[targetId].targetPosition.x << ',' << berth[targetId].targetPosition.y << " pos:"
             << position.x << ' ' << position.y << " next: " << nextPoint.x << ',' << nextPoint.y
             << " path:" << robotPath[id].step << '/' << robotPath[id].length
             << " gtg:" << goodsToGet.position.x << ',' << goodsToGet.position.y
             << " carrying:" << carrying << " waitTime:" << waitTime << " enable:" << enable << "任务：";
        if (mission == RobotState::MISSION_PULL) cerr << "PULL";
        if (mission == RobotState::MISSION_GET) cerr << "GET";
        if (mission == RobotState::MISSION_MOVE) cerr << "MOVE";
        cerr << " 状态:";
        if (state == RobotState::MISSION_GET) cerr << "GET";
        if (state == RobotState::MISSION_PULL) cerr << "PULL";
        if (state == RobotState::MISSION_MOVE) cerr << "MOVE";
        cerr << "\n";
    }
}
void Robot::update() {
    cerr << "robot[" << id << "] update\n";
    if (position == lastPosition && state == RobotState::MISSION_MOVE) {
        waitTime++;
    } else {
        waitTime = 0;
    }
    if (waitTime >= 3) {
        if (carrying) totGiveUp += goodsToGet.value;
        if (cerrRobot && cerrSwitch) cerr << "[" << id << "]放弃任务！总价值：<" << totGiveUp << ">\n";
        crashed = true;
        waitTime = 0;
        if (carrying) findBerth();
        else robotGetMission();
        return;
    }
//    maze[position.x][position.y] = PointState::PLAIN;
    if (state == RobotState::MISSION_MOVE) {
        moveStep();
    } else if (state == RobotState::MISSION_GET) {
        get();
        totGetValue += goodsToGet.value;
        if (cerrTotalGetValue && cerrSwitch) cerr << "totGetValue:" << totGetValue << '\n';
        state = RobotState::MISSION_MOVE;
        mission = RobotState::MISSION_PULL;
        findBerth();
    } else if (state == RobotState::MISSION_PULL) {
//        cerr << id << "Pulled\n";
        pull();
        berth[targetId].pushGoods(goodsToGet);
//        mission = state = RobotState::FREE;
        robotGetMission();
//        moveStep();
    }
    if (state == RobotState::FREE) {
        robotGetMission();
    }
}
void Robot::moveStep() {
    if (robotPath[id].length > 300) {
//        mission = state = RobotState::FREE;
        if (mission == RobotState::MISSION_GET) {
            robotGetMission();
        } else {
            findBerth();
        }
        crashed = true;
    }
    if (position == nextPoint || nextPoint == Point(-1, -1)) {
        nextPoint = robotPath[id].getNextPoint();
        if (cerrSwitch && cerrRobot) cerr << "robot[" << id << "] getNextPoint" << nextPoint.x << ',' << nextPoint.y << '\n';
    }
    if ((mission == RobotState::MISSION_GET && goodsToGet.position == position) || (mission == RobotState::MISSION_PULL && berth[targetId].position == position)) {
//              || (mission == RobotState::MISSION_PULL && maze[position.x][position.y] == 'B')) {
        state = mission;
        return;
    }
    if (avoidCrash) {
        for (int i = 0; i < 10; i++) {
            if (i == id) continue;
            if ((nextPoint != Point(-1, -1)) &&
                (robot[i].position == nextPoint || nextPoint == robot[i].nextPoint)) {
                crashed = true;
                if (cerrRobot && cerrSwitch)
                    cerr << "阻止了" << id << "移动到" << i << "的位置\n";
//                if (i < id || robotPath[i].length > 100000) {
                    if (mission == RobotState::MISSION_GET) {
                        robotGetMission();
                    }
                    if (mission == RobotState::MISSION_PULL) {
                        findBerth();
                    }
//                }
                return;
            }
        }
    }
    crashed = false;
    while (nextPoint != Point(-1, -1) && nextPoint == position) nextPoint = robotPath[id].getNextPoint();
    if (nextPoint != Point(-1, -1)) move(position.getDirection(nextPoint));
}
Robot robot[10];

void Robot::getNearRobot(Goods goods){
    if(nearRobotId[goods.id] >= 0 || nearRobotId[goods.id] == -2)return;
    int mindis = 100000, nearId = -2, nowId;
    for(int i = 0; i <= 9; i++){
        if (robotFirstMission[i])nowId = -2;
        else nowId = robot[i].getTargetId();
        if(robotHome[i] == -1||nowId == -1)continue;
        int dis;
        if (nowId == -2) dis += pathLengthToStart[id][goods.position.x][goods.position.y];
        else dis += pathLengthToBerth[id][position.x][position.y];
        if(dis < mindis){
            mindis = dis;
            nearId = i;
        }
    }
    nearRobotId[goods.id] = nearId;
    nearRobotDis[goods.id] = mindis;
}
void Robot::robotGetMission() {
    if (nowFrameUse && frame <= 100) return;
    nowFrameUse = true;
    const int nx[]={0,0,1,-1};
    const int ny[]={1,-1,0,0};
    char thismap[200][200];
    int distance[200][200], stepnum[200][200], step[200][200], visit[200][200];
    int goodsHere[200][200];
    queue<Point> q;
    Point start = position;
    int targetGoods = -1;
    q.push(start);
    for(int i = 0; i < 200; i++)
        for(int j = 0; j < 200 ; j++)
            distance[i][j] = 1000000, goodsHere[i][j] = stepnum[i][j] = step[i][j] = visit[i][j] = 0, thismap[i][j] = maze[i][j];
    int sz = goodsOnMap.size();
    for (int i = 0; i < sz; i++) {
        goodsHere[goodsOnMap[i].position.x][goodsOnMap[i].position.y] = i;
    }
    distance[start.x][start.y] = 0;
    bool flag = false;
    for (int i = 0; i < 10; i++) {
        if (robot[i].crashed && i != id) thismap[robot[i].position.x][robot[i].position.y] = PointState::BLOCK;
    }
    while(!q.empty()) {
        Point cur = q.front();
        q.pop();
        int nextframe = step[cur.x][cur.y] + 1;
        for (int i = 0; i < 10; i++) {
            if (i != id) {
                if (robotPath[i].length > 50000) continue;
                if (!crashed && !robot[i].crashed) {
                    Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                    Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
                    Point robotThisPoint2 = robotPath[i].getPointbyTime(nextframe + 1);
                    if (robotThisPoint0 != Point(-1, -1))
                        thismap[robotThisPoint0.x][robotThisPoint0.y] = PointState::BLOCK;
                    if (robotThisPoint1 != Point(-1, -1))
                        thismap[robotThisPoint1.x][robotThisPoint1.y] = PointState::BLOCK;
                    if (robotThisPoint2 != Point(-1, -1))
                        thismap[robotThisPoint2.x][robotThisPoint2.y] = PointState::BLOCK;
                }
            }
        }
        for(int i = 0; i <= 3; i++){
            int dx = cur.x + nx[i];
            int dy = cur.y + ny[i];
            if(dx < 0||dx >= 200||dy < 0||dy >= 200)continue;
            if (visit[dx][dy]) continue;
            if (thismap[dx][dy] == PointState::OCEAN || thismap[dx][dy] == PointState::BLOCK)continue;
            if(distance[dx][dy] > distance[cur.x][cur.y] + 1){
                q.push((Point{dx,dy}));
                distance[dx][dy] = distance[cur.x][cur.y] + 1;
                visit[dx][dy] = 1;
                step[dx][dy] = step[cur.x][cur.y] + 1;
                stepnum[dx][dy] = i;
                if (goodsHere[dx][dy] != 0) {
                    if (mapNUM == 2 && (id <= 4) && (dx >= 98)) continue;
                    if (mapNUM == 2 && (id >= 5) && (dx < 98)) continue;
                    if (visitGoods[goodsOnMap[goodsHere[dx][dy]].id]) continue;
                    targetGoods = goodsHere[dx][dy];
                    flag = true;
                    break;
                }
//                if (distance[dx][dy] > 200) continue;
            }
        }
        for (int i = 0; i < 10; i++) {
            if (i != id) {
                if (robotPath[i].length > 50000) continue;
                if (!crashed && !robot[i].crashed) {
                    Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                    Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
                    Point robotThisPoint2 = robotPath[i].getPointbyTime(nextframe + 1);
                    if (robotThisPoint0.x != -1 && robotThisPoint0.y != -1)
                        thismap[robotThisPoint0.x][robotThisPoint0.y] = maze[robotThisPoint0.x][robotThisPoint0.y];
                    if (robotThisPoint1.x != -1 && robotThisPoint1.y != -1)
                        thismap[robotThisPoint1.x][robotThisPoint1.y] = maze[robotThisPoint1.x][robotThisPoint1.y];
                    if (robotThisPoint2.x != -1 && robotThisPoint2.y != -1)
                        thismap[robotThisPoint2.x][robotThisPoint2.y] = maze[robotThisPoint2.x][robotThisPoint2.y];
                }
            }
        }
        if (flag) break;
    }
    if(flag) {
        int dx = goodsOnMap[targetGoods].position.x, dy = goodsOnMap[targetGoods].position.y;
        int length = 0;
        stack<Point> repath;
        vector<Point> points;
//        cerr << "这是" << id << "的路径:" << "\n";
        while (dx != robot[id].position.x || dy != robot[id].position.y) {
            repath.push((Point){dx, dy});
//            cerr << dx << "," << dy << "  ";
            int lastx = dx, lasty = dy;
            dx -= nx[stepnum[lastx][lasty]];
            dy -= ny[stepnum[lastx][lasty]];
            length++;
        }
//        cerr << "\n";
        repath.push((Point){dx, dy});
        length++;
        while (!repath.empty()) {
            points.push_back(repath.top());
//        cerr << "repath top:" << repath.top().x << ',' << repath.top().y << '\n';
            repath.pop();
        }
        robotPath[id] = ((Path){points, length});
        visitGoods[goodsOnMap[targetGoods].id] = true;
        if (cerrSwitch && cerrRobot) cerr << "got Mission" << targetGoods << '\n';
        robot[id].setMission(goodsOnMap[targetGoods]);
//        cerr << "get target path ";
//        if (PathAlgorithm == 1) robotPath[id] = getPathbyAStar(id, goodsOnMap[targetGoods].position);
//        else robotPath[id] = getPath1(id, goodsOnMap[targetGoods].position);
        robotFirstMission[id] = false;
        nextPoint = robotPath[id].getNextPoint();
    }
}
void Robot::findBerth() {
    cerr << "find start!\n";
    if (nowFrameUse && frame <= 100) return;
    nowFrameUse = true;
    const int nx[]={0,0,1,-1};
    const int ny[]={1,-1,0,0};
    char thismap[200][200];
    int distance[200][200], stepnum[200][200], step[200][200], visit[200][200];
    int berthHere[200][200];
    queue<Point> q;
    Point start = position;
    q.push(start);
    for(int i = 0; i < 200; i++)
        for(int j = 0; j < 200 ; j++)
            distance[i][j] = 1000000, berthHere[i][j] = -1, step[i][j] = visit[i][j] = 0, stepnum[i][j] = -1, thismap[i][j] = maze[i][j];
    for (int i = 0; i < 10; i++) {
        if (!berthVisitable[i]) continue;
        berthHere[berth[i].targetPosition.x][berth[i].targetPosition.y] = i;
    }
    distance[start.x][start.y] = 0;
    bool flag = false;
    for (int i = 0; i < 10; i++) {
        if (robot[i].crashed && i != id) thismap[robot[i].position.x][robot[i].position.y] = PointState::BLOCK;
    }
    while(!q.empty()) {
        Point cur = q.front();
        q.pop();
        int nextframe = step[cur.x][cur.y] + 1;
        for (int i = 0; i < 10; i++) {
            if (i != id) {
                if (robotPath[i].length > 50000) continue;
                Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
                Point robotThisPoint2 = robotPath[i].getPointbyTime(nextframe + 1);
                if (robotThisPoint0 != Point(-1, -1)) thismap[robotThisPoint0.x][robotThisPoint0.y] = PointState::BLOCK;
                if (robotThisPoint1 != Point(-1, -1)) thismap[robotThisPoint1.x][robotThisPoint1.y] = PointState::BLOCK;
                if (robotThisPoint2 != Point(-1, -1)) thismap[robotThisPoint2.x][robotThisPoint2.y] = PointState::BLOCK;
            }
        }
        for(int i = 0; i <= 3; i++){
            int dx = cur.x + nx[i];
            int dy = cur.y + ny[i];
            if(dx < 0||dx >= 200||dy < 0||dy >= 200)continue;
            if (thismap[dx][dy] == PointState::OCEAN || thismap[dx][dy] == PointState::BLOCK) continue;
            if(distance[dx][dy] > distance[cur.x][cur.y] + 1){
                q.push((Point{dx,dy}));
                distance[dx][dy] = distance[cur.x][cur.y] + 1;
                visit[dx][dy] = 1;
                step[dx][dy] = step[cur.x][cur.y] + 1;
                stepnum[dx][dy] = i;
                if (berthHere[dx][dy] != -1) {
                    targetId = berthHere[dx][dy];
                    flag = true;
                    break;
                }
            }
        }
        for (int i = 0; i < 10; i++) {
            if (i != id) {
                if (robotPath[i].length > 50000) continue;
                Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
                Point robotThisPoint2 = robotPath[i].getPointbyTime(nextframe + 1);
                if (robotThisPoint0.x != -1 && robotThisPoint0.y != -1) thismap[robotThisPoint0.x][robotThisPoint0.y] = maze[robotThisPoint0.x][robotThisPoint0.y];
                if (robotThisPoint1.x != -1 && robotThisPoint1.y != -1) thismap[robotThisPoint1.x][robotThisPoint1.y] = maze[robotThisPoint1.x][robotThisPoint1.y];
                if (robotThisPoint2.x != -1 && robotThisPoint2.y != -1) thismap[robotThisPoint2.x][robotThisPoint2.y] = maze[robotThisPoint2.x][robotThisPoint2.y];

            }
        }
        if (flag) break;
    }
//    if (frame > 300) {
//        cerr << "id:" << id << "\n";
//        for (int i = 0; i < 200; i++) {
//            for (int j = 0; j < 200; j++)
//                cerr << stepnum[i][j];
//            cerr << "\n";
//        }
//    }
    if (flag && targetId != -1) {
//        int dx = berth[targetId].position.x, dy = berth[targetId].position.y;
//        int length = 0;
//        stack<Point> repath;
//        vector<Point> points;
//        cerr << "这是" << id << "的路径:" << "\n";
//        cerr << dx << " " << dy << " " << targetId << "\n" ;
//        cerr << "length:" << step[dx][dy] << "\n";
//        while (dx != robot[id].position.x || dy != robot[id].position.y) {
//            repath.push((Point){dx, dy});
////            cerr << dx << "," << dy << "  ";
//            int lastx = dx, lasty = dy;
//            dx -= nx[stepnum[lastx][lasty]];
//            dy -= ny[stepnum[lastx][lasty]];
//            length++;
//        }
//        cerr << "\n";
//        repath.push((Point){dx, dy});
//        length++;
//        while (!repath.empty()) {
//            points.push_back(repath.top());
////        cerr << "repath top:" << repath.top().x << ',' << repath.top().y << '\n';
//            repath.pop();
//        }
//        robotPath[id] = ((Path){points, length});
        if (PathAlgorithm == 1) robotPath[id] = getPathbyAStar(id, berth[targetId].position);
        else robotPath[id] = getPath1(id, berth[targetId].position);
        mission = RobotState::MISSION_PULL;
        state = RobotState::MISSION_MOVE;
    }
}
void makeMap(vector<Point> points) {
//    freopen("Pathmap.txt", "w", stderr);
    char pathinMap[300][300];
    for (int i = 0; i < 200; i++)
        for (int j = 0; j < 200; j++)
            pathinMap[i][j] = maze[i][j];
    for (int i = 0; i < points.size(); i++)
        pathinMap[points[i].x][points[i].y] = '~';
//    for (int i = 0; i < 200; i++) {
//        for (int j = 0; j < 200; j++)
//            cerr << pathinMap[i][j];
//        cerr << "\n";
//    }
//    fclose(stderr);
}
Path getPath1(int robId, Point target) {
//    cerr << "使用了 Algorithm BFS \n";
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
    for(int i = 0; i < 10; i++) {
        if (i == robId) continue;
        if (!robot[i].crashed) continue;
        thismap[robot[i].position.x][robot[i].position.y] = PointState::BLOCK;
    }
    while (!q.empty()) {
        Point fr = q.front();
        q.pop();
        int nextframe = step[fr.x][fr.y] + 1;
        for (int i = 0; i < 10; i++) {
            if (i == robId) continue;
        }
        for (int i = 0; i < 10; i++) {
            if (i != robId) {
                if (robotPath[i].length > 50000) continue;
                Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
                Point robotThisPoint2 = robotPath[i].getPointbyTime(nextframe + 1);
                if (robotThisPoint0 != Point(-1, -1)) thismap[robotThisPoint0.x][robotThisPoint0.y] = PointState::BLOCK;
                if (robotThisPoint1 != Point(-1, -1)) thismap[robotThisPoint1.x][robotThisPoint1.y] = PointState::BLOCK;
                if (robotThisPoint2 != Point(-1, -1)) thismap[robotThisPoint2.x][robotThisPoint2.y] = PointState::BLOCK;
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
                Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
                Point robotThisPoint2 = robotPath[i].getPointbyTime(nextframe + 1);
                if (robotThisPoint0.x != -1 && robotThisPoint0.y != -1) thismap[robotThisPoint0.x][robotThisPoint0.y] = maze[robotThisPoint0.x][robotThisPoint0.y];
                if (robotThisPoint1.x != -1 && robotThisPoint1.y != -1) thismap[robotThisPoint1.x][robotThisPoint1.y] = maze[robotThisPoint1.x][robotThisPoint1.y];
                if (robotThisPoint2.x != -1 && robotThisPoint2.y != -1) thismap[robotThisPoint2.x][robotThisPoint2.y] = maze[robotThisPoint2.x][robotThisPoint2.y];

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
        nx -= fxx[stepnum[lastx][lasty]];
        ny -= fyy[stepnum[lastx][lasty]];
        length++;
    }
    repath.push((Point){nx, ny});
    length++;
    while (!repath.empty()) {
        points.push_back(repath.top());
//        cerr << "repath top:" << repath.top().x << ',' << repath.top().y << '\n';
        repath.pop();
    }
//    makeMap(points);
//    cerr << "pathLength" << length << '\n';
    return Path(points, length);
}
Path getPathbyAStar(int robId, Point target) {
    if (cerrSwitch && cerrRobot) cerr << "robot[" << robId << "] 使用了 Algorithm A*" << "\n";
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
    q.push((node){robot[robId].position, target, 0});
    int b[300][300], dirc[300][300];
    char thismap[300][300];
    for (int i = 0; i < 210; i++)
        for (int j = 0; j < 210; j++) {
            b[i][j] = dirc[i][j] = 0;
            thismap[i][j] = maze[i][j];
        }
    for (int i = 0; i < 10; i++) {
        if (i == robId) continue;
        thismap[robot[i].position.x][robot[i].position.y] = PointState::BLOCK;
    }
    int flag = 0, length = 0;
    while (!q.empty()) {
        node tp = q.top();
        q.pop();
        /*
         * getRobotPathArea
         * */
        int nextframe = tp.step + 1;
        for (int i = 0; i < 10; i++)
            if (i != robId) {
                if (robotPath[i].length > 50000) continue;
                Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
                Point robotThisPoint2 = robotPath[i].getPointbyTime(nextframe + 1);
                if (robotThisPoint0 != Point(-1, -1)) thismap[robotThisPoint0.x][robotThisPoint0.y] = PointState::BLOCK;
                if (robotThisPoint1 != Point(-1, -1)) thismap[robotThisPoint1.x][robotThisPoint1.y] = PointState::BLOCK;
                if (robotThisPoint2 != Point(-1, -1)) thismap[robotThisPoint2.x][robotThisPoint2.y] = PointState::BLOCK;
            }

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

        for (int i = 0; i < 10; i++) {
            if (i != robId) {
                if (robotPath[i].length > 50000) continue;
                Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
                Point robotThisPoint2 = robotPath[i].getPointbyTime(nextframe + 1);
                if (robotThisPoint0.x != -1 && robotThisPoint0.y != -1) thismap[robotThisPoint0.x][robotThisPoint0.y] = maze[robotThisPoint0.x][robotThisPoint0.y];
                if (robotThisPoint1.x != -1 && robotThisPoint1.y != -1) thismap[robotThisPoint1.x][robotThisPoint1.y] = maze[robotThisPoint1.x][robotThisPoint1.y];
                if (robotThisPoint2.x != -1 && robotThisPoint2.y != -1) thismap[robotThisPoint2.x][robotThisPoint2.y] = maze[robotThisPoint2.x][robotThisPoint2.y];
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
    while (nx != robot[robId].position.x || ny != robot[robId].position.y) {
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
int Robot::getMission() {
    return mission;
}
Goods Robot::getGoodsToGet() {
    return goodsToGet;
}
int Robot::getTargetId() {
    return targetId;
}
void Robot::move(int direction) {
    cout << "move " << id << ' ' << direction << '\n';
}
void Robot::get() {
    cout << "get " << id << '\n';
    if (cerrSwitch && cerrRobot) cerr << "robot[" << id << "] get!\n";
}
void Robot::pull() {
    cout << "pull " << id << '\n';
    if (cerrSwitch && cerrRobot) cerr << "robot[" << id << "] pull!\n";
}