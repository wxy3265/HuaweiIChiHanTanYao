//
// Created by 86187 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_PATH_H
#define HUAWEIICHIHANTANYAO_PATH_H
#include "tanyao.h"
const int fx[4] = {0, 0, 1, -1};
const int fy[4] = {1, -1, 0, 0};

class Path {
    vector points;
    int lengths, step;
    Path() {

    }
    Path(Point start, Point end) {
        step = 0, lengths = 0;
        queue<Point> q;
        q.push(start);
        int ffind[300][300], steps[300][300];
        for (int i = 0; i < 210; i++)
            for (int j = 0; j < 210; j++)
                ffind[i][j] = steps[i][j] = 0;
//        ffind[start.x][start.y] = 0;
        int flag = 0;
        while (!q.empty()) {
            Point p = q.front();
            q.pop();
//            if (ffind[p.x][p.y]) continue;
//            ffind[p.x][p.y] = 1;
            for (int i = 0; i < 4; i++) {
                int ex = p.x + fx[i], ey = p.y + fy[i];
                if (ex < 0 || ex > 199 || ey < 0 || ey > 199) continue;
                if (ffind[ex][ey]) continue;
                if (maze[ex][ey] != '*' && maze[ex][ey] != '#') {
                    q.push((Point){ex, ey});
                    ffind[ex][ey] = 1;
                    steps[ex][ey] = i;
                    if (ex == end.x && ey == end.y) {
                        flag = 1;
                        break;
                    }
                }
            }
            if (flag) break;
        }
        int nx = end.x, ny = end.y;
        stack<Point> repath;
        while (nx != start.x && ny != start.y) {
            repath.push((Point){nx, ny});
            int lastx = nx, lasty = ny;
            nx -= fx[steps[lastx][lasty]];
            ny -= fy[steps[lastx][lasty]];
            lengths++;
        }
        while (!repath.empth()) {
            points.push(repath.top());
            repath.pop();
        }
    }
    Point getNextPoint() {
        return points[++step];
    }
};


#endif //HUAWEIICHIHANTANYAO_PATH_H