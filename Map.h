//
// Created by 86187 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_MAP_H
#define HUAWEIICHIHANTANYAO_MAP_H
#include "tanyao.h"

class Map {
    char maze[300][300];

    void init() {
        int cnt = 0;
        for (int i = 0; i < 200; i++)
            for (int j = 0; j < 200; j++){
                cin >> maze[i][j];
                if (maze[i][j] == 'A') {
                    rob[cnt].ID = cnt;
                    rob[cnt].position.x = i;
                    rob[cnt].position.y = j;
                }
//                if (maze[i][j] == '')
            }
        for (int i = 0, id, x, y, time, vel; i <= 9; i++) {
            scanf("%d%d%d%d%d", &id, &x, &y, &time, &vel);
            ber[id].ID = id, ber[id].x = x, ber[id].y = y, ber[id].time = time, ber[id].velocity = vel;
        }
        scanf("%d", &Ship.capacity);
        string s;
        cin >> s;
        cout << s << "\n";
    }
    void update() {
        scanf("%d%d", &frame, &totalMoney);
        int k;
        scanf("%d", &k);
        for (int i = 1, x, y, m; i <= k; i++) {
            scanf("%d%d%d", &x, &y, &m);

        }
        for (int i = 0, state, x, y, goods; i < 10; i++) {
            scanf("%d%d%d%d", &goods, &x, &y, &state);

        }
        for (int i = 0, state, id; i < 5; i++) {
            scanf("%d%d", &state, &id);

        }
        string thisisOK;
        cin >> thisisOK;

        cout << thisisOK << "\n";
    }
};


#endif //HUAWEIICHIHANTANYAO_MAP_H
