//
// Created by wxy3265 on 2024/3/7.
//

#include "Robot.h"

void makeMap(vector<Point> points);

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
int Robot::getState() {return state;}
void Robot::redirection() {
    this->mission = RobotState::MISSION_PULL;
    robotPath[this->id] = getPathbyAStar(this->id,  berth[nearBerthId[this->position.x][this->position.y]].position);

}
void Robot::setMission(Goods _goodsToGet, int _targetId) {
    goodsToGet = _goodsToGet;
    targetId = _targetId;
    mission = RobotState::MISSION_GET;
    state = RobotState::MISSION_MOVE;
    nextPoint = position;
    if (PathAlgorithm == 0) robotPath[id] = getPath1(id, goodsToGet.position);
    else if (PathAlgorithm == 1) robotPath[id] = getPathbyAStar(id, goodsToGet.position);
    else robotPath[id] = getPath1(id, goodsToGet.position);
}

int totGetValue = 0;
int totGiveUp = 0;
//更新机器人状态
void Robot::update(Point _position, bool _enable, bool _carrying) {
    lastPosition = position;
    position = _position;
    enable = _enable;
    carrying = _carrying;
    if (position == lastPosition && state == RobotState::MISSION_MOVE) {
        waitTime++;
    } else {
        waitTime = 0;
    }
    if (waitTime >= 10) {
        if (carrying) totGiveUp += goodsToGet.value;
        if (cerrRobot && cerrSwitch) cerr << "[" << id << "]放弃任务！总价值：<" << totGiveUp << ">\n";
        waitTime = 0;
        state = RobotState::FREE;
        pull();
        return;
    }
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
    maze[position.x][position.y] = PointState::PLAIN;
    if (state == RobotState::MISSION_MOVE) {
        moveStep();
    } else if (state == RobotState::MISSION_GET) {
        get();
//        if (!carrying) { return; }
        totGetValue += goodsToGet.value;
        if (cerrTotalGetValue && cerrSwitch)
            cerr << "totGetValue:" << totGetValue << '\n';
        nextPoint = robotPath[id].getNextPoint();
        state = RobotState::MISSION_MOVE;
        mission = RobotState::MISSION_PULL;
        if (PathAlgorithm == 0) robotPath[id] = getPath1(id, berth[targetId].targetPosition);
        else if (PathAlgorithm == 1) robotPath[id] = getPathbyAStar(id, berth[targetId].targetPosition);
        else robotPath[id] = getPath1(id, berth[targetId].targetPosition);
        moveStep();
    } else if (state == RobotState::MISSION_PULL) {
//        cerr << id << "Pulled\n";
        pull();
//        if (carrying) return;
        berth[targetId].pushGoods(goodsToGet);
        mission = state = RobotState::FREE;
        moveStep();
    }
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

void Robot::moveStep() {
    if (robotPath[id].length > 500000) {
        if (mission == RobotState::MISSION_GET) {
            if (PathAlgorithm == 0) robotPath[id] = getPath1(id, goodsToGet.position);
            else if (PathAlgorithm == 1) robotPath[id] = getPathbyAStar(id, goodsToGet.position);
            else robotPath[id] = getPath1(id, goodsToGet.position);
        }
        if (mission == RobotState::MISSION_PULL) {
            if (PathAlgorithm == 0) robotPath[id] = getPath1(id, berth[targetId].targetPosition);
            else if (PathAlgorithm == 1) robotPath[id] = getPathbyAStar(id, berth[targetId].targetPosition);
            else robotPath[id] = getPath1(id, berth[targetId].targetPosition);
        }
        return;
    }
    if (position == nextPoint || nextPoint == Point(-1, -1)) {
        nextPoint = robotPath[id].getNextPoint();
//            cerr << "getNextPoint" << nextPoint.x << ',' << nextPoint.y << '\n';
    }
    if (nextPoint == Point(-1, -1) || robotPath[id].length == robotPath[id].step) {
//              || (mission == RobotState::MISSION_PULL && maze[position.x][position.y] == 'B')) {
        state = mission;
        return;
    }
    if (avoidCrash) {
        for (int i = 0; i < 10; i++) {
            if (i == id) continue;
            if ((nextPoint != Point(-1, -1)) &&
                (robot[i].position == nextPoint || nextPoint == robot[i].nextPoint)) {
                if (cerrRobot && cerrSwitch)
                    cerr << "阻止了" << id << "移动到" << i << "的位置\n";
                maze[position.x][position.y] = PointState::BLOCK;
                if (i < id) {
                    if (mission == RobotState::MISSION_GET) {
                        if (PathAlgorithm == 0) robotPath[id] = getPath1(id, goodsToGet.position);
                        else if (PathAlgorithm == 1) robotPath[id] = getPathbyAStar(id, goodsToGet.position);
                        else robotPath[id] = getPath1(id, goodsToGet.position);
                    }
                    if (mission == RobotState::MISSION_PULL) {
                        if (PathAlgorithm == 0) robotPath[id] = getPath1(id, berth[targetId].targetPosition);
                        else if (PathAlgorithm == 1) robotPath[id] = getPathbyAStar(id, berth[targetId].targetPosition);
                        else robotPath[id] = getPath1(id, berth[targetId].targetPosition);
                    }
                    nextPoint = robotPath[id].getNextPoint();
                }
                return;
            }
        }
    }
    move(position.getDirection(nextPoint));
}

Robot robot[10];

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
//    for(int i = 0; i < 10; i++) {
//        if (i == robId) continue;
//        thismap[robot[i].position.x][robot[i].position.y] = PointState::BLOCK;
//    }
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
//                Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
//                Point robotThisPoint2 = robotPath[i].getPointbyTime(nextframe + 1);
//                if (robotThisPoint0 != Point(-1, -1)) thismap[robotThisPoint0.x][robotThisPoint0.y] = PointState::BLOCK;
                if (robotThisPoint1 != Point(-1, -1)) thismap[robotThisPoint1.x][robotThisPoint1.y] = PointState::BLOCK;
//                if (robotThisPoint2 != Point(-1, -1)) thismap[robotThisPoint2.x][robotThisPoint2.y] = PointState::BLOCK;
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
//                Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
//                Point robotThisPoint2 = robotPath[i].getPointbyTime(nextframe + 1);
//                if (robotThisPoint0.x != -1 && robotThisPoint0.y != -1) thismap[robotThisPoint0.x][robotThisPoint0.y] = maze[robotThisPoint0.x][robotThisPoint0.y];
                if (robotThisPoint1.x != -1 && robotThisPoint1.y != -1) thismap[robotThisPoint1.x][robotThisPoint1.y] = maze[robotThisPoint1.x][robotThisPoint1.y];
//                if (robotThisPoint2.x != -1 && robotThisPoint2.y != -1) thismap[robotThisPoint2.x][robotThisPoint2.y] = maze[robotThisPoint2.x][robotThisPoint2.y];

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
//    cerr << "使用了 Algorithm A*" << "\n";
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
//                Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
//                if (robotThisPoint0 != Point(-1, -1)) thismap[robotThisPoint0.x][robotThisPoint0.y] = PointState::BLOCK;
                if (robotThisPoint1 != Point(-1, -1)) thismap[robotThisPoint1.x][robotThisPoint1.y] = PointState::BLOCK;
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
//                Point robotThisPoint0 = robotPath[i].getPointbyTime(nextframe - 1);
                Point robotThisPoint1 = robotPath[i].getPointbyTime(nextframe);
//                if (robotThisPoint0.x != -1 && robotThisPoint0.y != -1) thismap[robotThisPoint0.x][robotThisPoint0.y] = maze[robotThisPoint0.x][robotThisPoint0.y];
                if (robotThisPoint1.x != -1 && robotThisPoint1.y != -1) thismap[robotThisPoint1.x][robotThisPoint1.y] = maze[robotThisPoint1.x][robotThisPoint1.y];
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
