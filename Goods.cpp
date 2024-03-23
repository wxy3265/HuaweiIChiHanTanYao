//
// Created by wxy3265 on 2024/3/9.
//

#include "Goods.h"
#include "tanyao.h"

vector<Goods> newGoods;
vector<Goods> goodsOnMap;

//void Goods::pretreatDis() {
//    queue<Point> q;
//    Point start = position;
//    q.push(start);
//    for(int i = 0; i < 200; i++)
//        for(int j = 0; j < 200 ; j++)
//            distance[i][j] = 1000000;
//    distance[start.x][start.y] = 0;
//    while(!q.empty()) {
//        Point cur = q.front();
//        q.pop();
//        for(int i = 0; i <= 3; i++){
//            const int nx[]={0,0,1,-1};
//            const int ny[]={1,-1,0,0};
//            int dx = cur.x + nx[i];
//            int dy = cur.y + ny[i];
//            if(dx < 0||dx >= 200||dy < 0||dy >= 200)continue;
//            if (maze[dx][dy] == PointState::OCEAN || maze[dx][dy] == PointState::BLOCK)continue;
//            if(distance[dx][dy] > distance[cur.x][cur.y] + 1){
//                distance[dx][dy] = distance[cur.x][cur.y] + 1;
//                q.push((Point{dx,dy}));
//            }
//        }
//    }
//}
//
//int Goods::getDis(Point point) {
//    return distance[point.x][point.y];
//}
