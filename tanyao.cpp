//
// Created by wxy3265 on 2024/3/8.
//

#include "tanyao.h"
char maze[300][300];
int frame, totalMoney, lastFrame, framesum;
int capacity;
const int PathAlgorithm = 1;//0为BFS 1为A*
const bool cerrRobot = false;
const bool cerrTotalGetValue = true;
const bool cerrShip = true;
const bool cerrFrame = false;
int nearBerthId[203][203];
int nearBerthLength[203][203];