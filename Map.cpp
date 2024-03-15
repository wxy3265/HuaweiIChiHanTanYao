//
// Created by wxy3265 on 2024/3/7.
//

#include "Map.h"

static int goodsNumber = 0;

const int nx[]={0,0,1,-1};
const int ny[]={1,-1,0,0};
int pathLengthToBerth[10][203][203];
int pathLengthToStart[10][203][203];
int berth_to_berth[10][10];
int pathLength[10][203][203];
int nearBerthId[203][203];
int nearBerthLength[203][203];
bool visitGoods[200007];

void Map::init() {
    for (int i = 0; i < 203; i++) {
        for (int j = 0; j < 203; j++) {
            nearBerthLength[i][j] = 1e7;
        }
    }
    //读入地图
    int cnt = 0;
    for (int i = 0; i < 200; i++) {
        for (int j = 0; j < 200; j++){
            cin >> maze[i][j];
            if (maze[i][j] == PointState::ROBOT) {
                robot[cnt].id = cnt;
                robot[cnt].position.x = i;
                robot[cnt].position.y = j;
                cnt++;
            }
        }
    }
    //读入泊位信息
    for (int i = 0, id, x, y, time, vel; i <= 9; i++) {
        scanf("%d%d%d%d%d", &id, &x, &y, &time, &vel);
        berth[id].id = id, berth[id].position.x = x, berth[id].position.y = y,
//        cerr << "berthid:" << id << " pos:" << berth[id].position.x << ',' << berth[id].position.y
//             << "time: " << time << " vel:" << vel << '\n';
        berth[id].distance = time, berth[id].velocity = vel;
        berth[id].distance = time, berth[id].velocity = vel;
        int flag = 0;
        for (int j = berth[id].position.x; j <= berth[id].position.x + 3; j++) {
            for (int k = berth[id].position.y; k <= berth[id].position.y + 3; k++) {
                for (int l = 0; l < 4; l++) {
                    const int nx[]={0,0,1,-1};
                    const int ny[]={1,-1,0,0};
                    int findx = nx[l] + j, findy = ny[l] + k;
                    if (findx < 0 || findx >= 200 || findy < 0 || findy >= 200) continue;
                    if (maze[findx][findy] == PointState::PLAIN) {
                        flag = 1;
                        berth[id].targetPosition = (Point){j,k};
                        break;
                    }
                }
                if (flag) break;
            }
            if (flag) break;
        }
//        cerr << "berth[" << id << "]" << " targetPosition:" << berth[id].targetPosition.x << "," << berth[id].targetPosition.y << '\n';
    }
    //初始化船舶ID
    for (int i = 0; i < 5; i++) ship[i].id = i;
    scanf("%d", &capacity);
    //OK
    string thisisOK;
    cin >> thisisOK;
    cout << thisisOK << "\n";
}

void cleanGoodsOnMap() {
//    cerr << "nowGoodsOnMap:" << goodsOnMap.size() << '\n';
    vector <Goods> tmp;
    tmp.clear();
    int n = goodsOnMap.size();
    for (int i = 0; i < n; i++) {
        Goods goods = goodsOnMap[i];
        if (visitGoods[goods.id] || frame >= goods.time + 1000) continue;
        tmp.emplace_back(goods);
//        cerr << "endGoodsOnMap:" << goodsOnMap.size() << '\n';
    }
    goodsOnMap.clear();
    n = tmp.size();
    for (int i = 0; i < n; i++) {
        Goods goods = tmp[i];
        goodsOnMap.emplace_back(goods);
    }
}

int totGoodsNumber = 0;
void Map::update() {
    scanf("%d%d", &frame, &totalMoney);
//    cerr << "lastFrame:" << lastFrame << " " << "frame:" << frame << '\n';
    framesum += frame - lastFrame - 1;
    lastFrame = frame;
    //读入新增货物
    int k;
    scanf("%d", &k);
    totGoodsNumber += k;
//    cerr << "TotGoodsNumber:" << totGoodsNumber << '\n';
    for (int i = 1, x, y, m; i <= k; i++) {
        scanf("%d%d%d", &x, &y, &m);
//        cerr << "newGoods Value:<" << m << ">\n";
        newGoods.emplace_back(Point(x, y), m, frame, ++goodsNumber);
        goodsOnMap.emplace_back(Point(x, y), m, frame, goodsNumber);
    }
    cleanGoodsOnMap();
    //更新机器人
    for (int i = 0, state, x, y, goods; i < 10; i++) {
        scanf("%d%d%d%d", &goods, &x, &y, &state);
        robot[i].update(Point(x, y), state, goods);
    }
//    cerr << "机器人更新完毕\n";
    //更新船舶
    for (int i = 0, state, id; i < 5; i++) {
        scanf("%d%d", &state, &id);
        ship[i].update(state);
    }
//    for (int i = 0; i < 10; i++) cerr << "berth:[" << i << "] goodsValue:<" << berth[i].getTotalValue() << ">\n";
    //OK
    string thisisOK;
    cin >> thisisOK;
    while(thisisOK != "OK") cin >> thisisOK;
}

int Map::getLengthFromBerthToPoint(int berthId, Point end) {
    return pathLengthToBerth[berthId][end.x][end.y];
}

int Map::getLengthFromStartToPoint(int robId, Point end) {
    return pathLengthToStart[robId][end.x][end.y];
}

int Map::getNearBerthId(Point point) {
    return nearBerthId[point.x][point.y];
}

void Map::pretreatPathToBerth(int berthId){
//    if (!open[berthId]) return;
    queue<Point> q;
    Point start = berth[berthId].position;
    q.push(start);
    for(int i = 0; i < 200; i++)
        for(int j = 0; j < 200 ; j++)
            pathLengthToBerth[berthId][i][j] = 1000000;
    pathLengthToBerth[berthId][start.x][start.y] = 0;
    while(!q.empty()) {
        Point cur = q.front();
        q.pop();
        for(int i = 0; i <= 3; i++){
            const int nx[]={0,0,1,-1};
            const int ny[]={1,-1,0,0};
            int dx = cur.x + nx[i];
            int dy = cur.y + ny[i];
            if(dx < 0||dx >= 200||dy < 0||dy >= 200)continue;
            if (maze[dx][dy] == PointState::OCEAN || maze[dx][dy] == PointState::BLOCK)continue;
            if(pathLengthToBerth[berthId][dx][dy] > pathLengthToBerth[berthId][cur.x][cur.y] + 1){
                pathLengthToBerth[berthId][dx][dy] = pathLengthToBerth[berthId][cur.x][cur.y] + 1;
                if (pathLengthToBerth[berthId][dx][dy] < nearBerthLength[dx][dy]) {
                    nearBerthLength[dx][dy] = pathLengthToBerth[berthId][dx][dy];
                    nearBerthId[dx][dy] = berthId;
                }
                q.push((Point{dx,dy}));
            }
        }
    }
    /*for (int i = 0; i < 200; i++) {
        for (int j = 0; j < 200; j++) {
            cerr << nearBerthId[i][j];
        }
        cerr << '\n';
    }*/
}
void Map::pretreatPathToStart(int robId){
//    if (!open[berthId]) return;
    queue<Point> q;
    Point start = robot[robId].position;
    q.push(start);
    for(int i = 0; i < 200; i++)
        for(int j = 0; j < 200 ; j++)
            pathLengthToStart[robId][i][j] = 1000000;
    pathLengthToStart[robId][start.x][start.y] = 0;
    while(!q.empty()) {
        Point cur = q.front();
        q.pop();
        for(int i = 0; i <= 3; i++){
            int dx = cur.x + nx[i];
            int dy = cur.y + ny[i];
            if(dx < 0||dx >= 200||dy < 0||dy >= 200)continue;
            if (maze[dx][dy] == PointState::OCEAN || maze[dx][dy] == PointState::BLOCK)continue;
            if(pathLengthToStart[robId][dx][dy] > pathLengthToStart[robId][cur.x][cur.y] + 1){
                pathLengthToStart[robId][dx][dy] = pathLengthToStart[robId][cur.x][cur.y] + 1;
//                if (pathLengthToStart[robId][dx][dy] < nearBerthLength[dx][dy]) {
//                    nearBerthLength[dx][dy] = pathLengthToStart[robId][dx][dy];
//                    nearBerthId[dx][dy] = robId;
//                }
                q.push((Point{dx,dy}));
            }
        }
    }
    /*for (int i = 0; i < 200; i++) {
        for (int j = 0; j < 200; j++) {
            cerr << nearBerthId[i][j];
        }
        cerr << '\n';
    }*/
}
bool berthCmp(Berth x,Berth y){
    return x.distance < y.distance;
}
bool Map::isOpen(int id) {
    return berthVisitable[id];
}
void Map::calcDistanceBetweenBerth(){
    berthVisitable[0] = true;
    berthVisitable[1] = true;
    berthVisitable[2] = true;
    berthVisitable[3] = true;
    berthVisitable[4] = true;
    berthVisitable[5] = true;
    berthVisitable[6] = true;
    berthVisitable[7] = true;
    berthVisitable[8] = true;
    berthVisitable[9] = true;
    return;
    int r = 50;
    int contain[12];
    bool close[12];
    memset(contain, 0, sizeof contain);
    memset(close, 0, sizeof close);
    for(int i = 0; i <= 9; i++)
        for(int j = 0; j <= 9; j++){
            if(i == j)continue;
            berth_to_berth[i][j] = getLengthFromBerthToPoint(i, berth[j].position);
            if(berth_to_berth[i][j] < r){
                contain[i]++;
            }
        }
    Berth newBerth[12];
    for(int i = 0; i <= 9; i++) newBerth[i] = berth[i];
    sort(newBerth, newBerth + 10, berthCmp);
    for(int i = 10; i >= 0; i--){
        for(int j = 0; j <= 9; j++){
            int id = newBerth[j].id;
            if (contain[id] == i && !close[id]){
                berthVisitable[id] = true;
                for(int k = 0; k <= 9; k++){
                    if(k == id)continue;
                    if(berth_to_berth[id][k] < r){
                        close[k] = true;
                    }
                }
            }
        }
    }
}