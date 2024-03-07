//
// Created by wxy3265 on 2024/3/7.
//

#include "Map.h"

void Map::init() {
    int cnt = 0;
    for (int i = 0; i < 200; i++) {
        for (int j = 0; j < 200; j++){
            cin >> maze[i][j];
            if (maze[i][j] == 'A') {
                robot[cnt].id = cnt;
                robot[cnt].position.x = i;
                robot[cnt].position.y = j;
                cnt++;
            }
        }
    }
//    while (true) cerr << robot[0].position.x;
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
    cerr << "read:" << frame << '\n';
    int k;
    scanf("%d", &k);
    for (int i = 1, x, y, m; i <= k; i++) {
        scanf("%d%d%d", &x, &y, &m);
//        while (true) cerr << "x:" << x << "y:" << y << '\n';
        newGoods.push_back(Goods(Point(x, y), m, frame));
    }
    for (int i = 0, state, x, y, goods; i < 10; i++) {
        scanf("%d%d%d%d", &goods, &x, &y, &state);
//        cerr << "robot:" << i << ' ' << robot[i].position.x << ' ' << robot[i].position.y << '\n';
        if (robot[i].getState() == RobotState::FREE) robot[i].setMission(Point(137, 117), Point(0, 0));
        robot[i].update(Point(x, y), state);
    }
    for (int i = 0, state, id; i < 5; i++) {
        scanf("%d%d", &state, &id);

    }
    string thisisOK;
    cin >> thisisOK;
    cout << thisisOK << "\n";
    cout.flush();
}
