//
// Created by wxy3265 on 2024/3/8.
//

#include "tanyao.h"
char maze[300][300];
int frame, totalMoney, lastFrame, framesum;
int capacity;
const bool cerrSwitch = true; //调试信息总开关
const bool cerrBerth = true; //港口调试信息
const bool cerrRobot = false; //机器人调试信息
const bool cerrTotalGetValue = true; //总获取金额调试信息
const bool cerrShip = true; //船舶调试信息
const bool cerrFrame = true; //帧标记调试信息
const bool avoidCrash = true;
const int PathAlgorithm = 1; //0为BFS 1为A*
int mostBerthNumber = 5;//可以连续到达的港口数量
const bool bindBerth = true; //机器人与港口绑定为true
const double deltaLength = 2;
const double deltaTime = 0;
const double deltaTarget = 0;
bool berthOpen[10] = {true, true, true, true, true, true, true, true, true, true};
bool berthOpenforShip[10] = {true, true, true, true, true, true, true, true, true, true};
int nearBerthId[203][203];
int nearBerthLength[203][203];