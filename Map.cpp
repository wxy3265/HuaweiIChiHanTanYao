//
// Created by wxy3265 on 2024/3/7.
//

#include "Map.h"

void Map::init() {
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
//    while (true) cerr << "cnt:" << cnt << '\n';
    for (int i = 0, id, x, y, time, vel; i <= 9; i++) {
        scanf("%d%d%d%d%d", &id, &x, &y, &time, &vel);
        berth[id].id = id, berth[id].position.x = x, berth[id].position.y = y,
        berth[id].distance = time, berth[id].velocity = vel;
    }
    scanf("%d", &capacity);
    string thisisOK;
    cin >> thisisOK;
    cout << thisisOK << "\n";
    cout.flush();
}

void Map::update() {
    scanf("%d%d", &frame, &totalMoney);
    cerr << "frame:" << frame << '\n';
    int k;
    scanf("%d", &k);
    for (int i = 1, x, y, m; i <= k; i++) {
        scanf("%d%d%d", &x, &y, &m);
//        while (true) cerr << "x:" << x << "y:" << y << '\n';
        newGoods.push_back(Goods(Point(x, y), m, frame));
        if (robot[0].getState() == RobotState::FREE) {
            cerr << "newGoods:" << newGoods.back().position.x << ',' << newGoods.back().position.y << '\n';
            if (Path(robot[0].position, newGoods.back().position, 0).length > 1000) continue;
            robot[0].setMission(newGoods.back(), berth[0]);
            newGoods.pop_back();
//            if (frame > 100) while (true) cerr << "f" << frame;
        }
    }
    for (int i = 0, state, x, y, goods; i < 10; i++) {
        scanf("%d%d%d%d", &goods, &x, &y, &state);
        cerr << "robot:" << i << ' ' << robot[i].position.x << ' ' << robot[i].position.y << ' '
             << "goods:" << goods << '\n';
//        if (robot[i].getState() == RobotState::FREE && i != 4) robot[i].setMission(Point(137, 117), Point(0, 0));
        robot[i].update(Point(x, y), state, goods);
    }
    for (int i = 0, state, id; i < 5; i++) {
        scanf("%d%d", &state, &id);
        ship[i].update(state);
        cerr << "ship state: " << state << ' ' << id << ' ' << ship[i].isFree() << '\n';
        if (ship[0].isFree()) {
            ship[0].setMission(berth[0]);
            if (frame > 100) while (true) ;
        }
    }
    string thisisOK;
    cin >> thisisOK;
    cout << thisisOK << "\n";
//    while (true) cerr << stdout << '\n';
    cout.flush();
}
