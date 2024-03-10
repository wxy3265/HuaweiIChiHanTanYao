//
// Created by wxy3265 on 2024/3/7.
//

#include "Map.h"

static int goodsNumber = 0;

const int nx[]={0,0,1,-1};
const int ny[]={1,-1,0,0};
int pathLength[12][207][207];
int berth_to_berth[12][12];
int open[12];

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
//    bool setOcean[300][300];
//    for (int i = 0; i < 200; i++)
//        for (int j = 0; j < 200; j++)
//            setOcean[i][j] = false;
//
//    for (int i = 0; i < 200; i++)
//        for (int j = 0; j < 200; j++)
//            if (maze[i][j] == PointState::OCEAN) {
//                for (int k = 0; k < 4; k++) {
//                    int ffx = i + nx[k], ffy = j + ny[k];
//                    if (maze[ffx][ffy] == PointState::BERTH && setOcean[i][j] == false)
//                        maze[ffx][ffy] = PointState::OCEAN, setOcean[ffx][ffy] = true;
//                }
//            }

        //读入泊位信息
        for (int i = 0, id, x, y, time, vel; i <= 9; i++) {
            scanf("%d%d%d%d%d", &id, &x, &y, &time, &vel);
            berth[id].id = id, berth[id].position.x = x, berth[id].position.y = y,
//        cerr << "berthid:" << id << " pos:" << berth[id].position.x << ',' << berth[id].position.y
//             << "time: " << time << " vel:" << vel << '\n';
            berth[id].distance = time, berth[id].velocity = vel;
        }
    //初始化船舶ID
    for (int i = 0; i < 5; i++) ship[i].id = i;
    for (int i = 0; i < 9; i++) robotEnable[i] = true;
    scanf("%d", &capacity);
    //OK
    string thisisOK;
    cin >> thisisOK;
    cout << thisisOK << "\n";
}

void Map::update() {
    scanf("%d%d", &frame, &totalMoney);
//    cerr << "lastFrame:" << lastFrame << " " << "frame:" << frame << '\n';
    framesum += frame - lastFrame - 1;
    lastFrame = frame;
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
    cerr << "机器人更新完毕\n";
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
bool berthCmp(Berth x,Berth y){
    return x.distance < y.distance;
}
bool Map::isOpen(int id) {
    return open[id];
}
void Map::calcDistanceBetweenBerth(){
    int r = 50;
    int contain[12];
    int close[12];
    memset(contain, 0, sizeof contain);
    memset(close, 0, sizeof close);
    for(int i = 0; i <= 9; i++)
        for(int j = 0; j <= 9; j++){
            if(i == j)continue;
            berth_to_berth[i][j] = getLength(i,berth[j].position);
            if(berth_to_berth[i][j] < r){
                contain[i]++;
            }
        }
    Berth newBerth[12];
    for(int i = 0; i <= 9; i++)newBerth[i] = berth[i];
    sort(newBerth, newBerth + 10, berthCmp);
    for(int i = 10; i >= 0; i--){
        for(int j = 0; j <= 9; j++){
            int id = newBerth[j].id;
            if (contain[id] == i && !close[id]){
                open[id] = 1;
                for(int k = 0; k <= 9; k++){
                    if(k == id)continue;
                    if(berth_to_berth[id][k] < r){
                        close[k] = 1;
                    }
                }
            }
        }
    }
}