//
// Created by wxy3265 on 2024/3/7.
//

#include "Map.h"

static int goodsNumber = 0;

const int nx[]={0,0,1,-1};
const int ny[]={1,-1,0,0};
int pathLength[12][207][207];

void Map::init() {
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
        cerr << "berthid:" << id << " pos:" << berth[id].position.x << ',' << berth[id].position.y
             << "time: " << time << " vel:" << vel << '\n';
        berth[id].distance = time, berth[id].velocity = vel;
    }
    //初始化船舶ID
    for (int i = 0; i < 5; i++) ship[i].id = i;
    scanf("%d", &capacity);
    //OK
    string thisisOK;
    cin >> thisisOK;
    cout << thisisOK << "\n";
}

void Map::update() {
    scanf("%d%d", &frame, &totalMoney);
    cerr << "frame:" << frame << '\n';
    //读入新增货物
    int k;
    scanf("%d", &k);
    for (int i = 1, x, y, m; i <= k; i++) {
        scanf("%d%d%d", &x, &y, &m);
        newGoods.push_back(Goods(Point(x, y), m, frame, ++goodsNumber));
    }
    //更新机器人
    for (int i = 0, state, x, y, goods; i < 10; i++) {
        scanf("%d%d%d%d", &goods, &x, &y, &state);
        robot[i].update(Point(x, y), state, goods);
    }
    //更新船舶
    for (int i = 0, state, id; i < 5; i++) {
        scanf("%d%d", &state, &id);
        ship[i].update(state);
    }
    //OK
    string thisisOK;
    cin >> thisisOK;
    while(thisisOK != "OK") cin >> thisisOK;
}

int Map::getLength(int berthId, Point end) {
    return pathLength[berthId][end.x][end.y];
}

void Map::pretreatPath(Berth ber){
    queue<Point> q;
    Point start = ber.position;
    q.push(start);
    for(int i = 0; i <= 200; i++)
        for(int j = 0; j <=200 ; j++)
            pathLength[ber.id][i][j] = 1000000;
    pathLength[ber.id][start.x][start.y] = 0;
    while(q.size()){
        Point cur = q.front();
        q.pop();
        for(int i = 0; i <= 3; i++){
            int dx = cur.x + nx[i];
            int dy = cur.y + ny[i];
            if(dx < 0||dx >= 200||dy < 0||dy >= 200)continue;
            if (maze[dx][dy] == PointState::OCEAN || maze[dx][dy] == PointState::BLOCK)continue;
            if(pathLength[ber.id][dx][dy] > pathLength[ber.id][cur.x][cur.y] + 1){
                pathLength[ber.id][dx][dy] = pathLength[ber.id][cur.x][cur.y] + 1;
                q.push((Point{dx,dy}));
            }
        }
    }
}