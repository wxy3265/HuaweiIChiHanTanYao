//
// Created by wxy3265 on 2024/3/7.
//

#ifndef HUAWEIICHIHANTANYAO_TANYAO_H
#define HUAWEIICHIHANTANYAO_TANYAO_H
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <queue>
#include <cmath>
#include <stack>
#include <map>
#pragma GCC optimize(2)

extern char maze[300][300];
extern int frame, totalMoney, lastFrame, framesum;
extern int capacity;
extern const int PathAlgorithm; //0为bfs 1为 A*
extern const bool cerrSwitch;
extern const bool cerrBerth;
extern const bool cerrRobot;
extern const bool cerrTotalGetValue;
extern const bool cerrShip;
extern const bool cerrFrame;
extern const double deltaLength;
extern const double deltaTime;
extern const double deltaTarget;
extern const bool bindBerth;
extern const bool berthOpen[10];
extern int nearBerthId[203][203];
extern int nearBerthLength[203][203];

using namespace std;

#endif //HUAWEIICHIHANTANYAO_TANYAO_H
