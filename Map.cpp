//
// Created by wxy3265 on 2024/3/7.
//

#include "Map.h"

void Map::init() {
    for(int i = 0; i < 10; i++) robot[i].id = i;
    int cnt = 0;
    for (int i = 0; i < 200; i++) {
        for (int j = 0; j < 200; j++){
            cin >> maze[i][j];
            if (maze[i][j] == 'A') {
                robot[cnt].id = cnt;
                robot[cnt].position.x = i;
                robot[cnt].position.y = j;
            }
        }
    }
    for (int i = 0, id, x, y, time, vel; i <= 9; i++) {
        scanf("%d%d%d%d%d", &id, &x, &y, &time, &vel);
        berth[id].id = id, berth[id].position.x = x, berth[id].position.y = y,
        berth[id].distance = time, berth[id].velocity = vel;
    }
    scanf("%d", &capacity);
//        while(true) cerr << "!";
    string thisisOK;
    cin >> thisisOK;
    cout << thisisOK << "\n";
    cout.flush();
//        while (true) cerr << "init:" << thisisOK;
}

void Map::update() {
    scanf("%d%d", &frame, &totalMoney);
    cerr << "read:" << frame << '\n';
    int k;
    scanf("%d", &k);
    for (int i = 1, x, y, m; i <= k; i++) {
        scanf("%d%d%d", &x, &y, &m);
        newGoods.push_back(Goods(Point(x, y), m, frame));
    }
    for (int i = 0, state, x, y, goods; i < 10; i++) {
        cerr << "robot:" << i << '\n';
        scanf("%d%d%d%d", &goods, &x, &y, &state);
        robot[i].update(Point(x, y), state);
    }
    for (int i = 0, state, id; i < 5; i++) {
        scanf("%d%d", &state, &id);

    }
    string thisisOK;
    cin >> thisisOK;
//        cerr << thisisOK << '\n';
    cout << thisisOK << "\n";
    cout.flush();
//        while(true) cerr << thisisOK;
}