//
// Created by 86187 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_PATH_H
#define HUAWEIICHIHANTANYAO_PATH_H
#include "tanyao.h"
#include "Point.h"
const int fx[4] = {0, 0, 1, -1};
const int fy[4] = {1, -1, 0, 0};

class Path {
public:
    vector<Point> points;
    int length, step;
    Path() {length = 1e7; step = 0;}

    Path(const vector<Point> &points, int length) : points(points), length(length) {step = 0;}

    Path(Point start, Point end, int id) {
        step = 0, length = 0;
        queue<Point> q;
        q.push(start);
        int ffind[300][300], steps[300][300];
        for (int i = 0; i < 210; i++)
            for (int j = 0; j < 210; j++)
                ffind[i][j] = steps[i][j] = 0;
        int flag = 0;
        while (!q.empty()) {
            Point p = q.front();
            q.pop();
            for (int i = 0; i < 4; i++) {
                int ex = p.x + fx[i], ey = p.y + fy[i];
                if (ex < 0 || ex > 199 || ey < 0 || ey > 199) continue;
                if (ffind[ex][ey]) continue;
                if (maze[ex][ey] != PointState::OCEAN && maze[ex][ey] != PointState::BLOCK) {
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
        if (!flag) {
            length = 10000000;
            return;
        }
        int nx = end.x, ny = end.y;
        stack<Point> repath;
        while (nx != start.x || ny != start.y) {
            repath.push((Point){nx, ny});
            int lastx = nx, lasty = ny;
            nx -= fx[steps[lastx][lasty]];
            ny -= fy[steps[lastx][lasty]];
            length++;
        }
        repath.push((Point){nx, ny});
        length++;
//        if (id != -1) {
//            cerr << "start:" << start.x << ',' << start.y << ' ' << "end:" << end.x << ',' << end.y << '\n';
//            cerr << "id:" << id << "path:";
//        }
        while (!repath.empty()) {
            points.push_back(repath.top());
//            if (id != -1) cerr << repath.top().x << ',' << repath.top().y << ' ';
            repath.pop();
        }
    }
    Point getNextPoint() {
        if (step == length - 1 || length > 50000) return {-1, -1};
        return points[++step];
    }
    Point getPointbyTime(int nextTime) {
        if (step + nextTime >= length || length > 50000) return {-1, -1};
        else {
            return points[step + nextTime];
        }
    }
};

extern Path robotPath[10];

#endif //HUAWEIICHIHANTANYAO_PATH_H
