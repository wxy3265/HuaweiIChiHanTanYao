//
// Created by wxy3265 on 2024/3/8.
//

#include "tanyao.h"
char maze[300][300];
int frame, totalMoney, lastFrame, framesum;
int capacity;
const bool cerrSwitch = false; //调试信息总开关
const bool cerrBerth = false; //港口调试信息
const bool cerrRobot = false; //机器人调试信息
const bool cerrTotalGetValue = true; //总获取金额调试信息
const bool cerrShip = false; //船舶调试信息
const bool cerrFrame = false; //帧标记调试信息
const int PathAlgorithm = 1; //0为BFS 1为A*
const bool bindBerth = true; //机器人与港口绑定为true
const double deltaLength = 3;
const double deltaTime = 0;
const double deltaTarget = 0;
const bool berthOpen[10] = {true, true, true, true, true, true, true, true, true, true};
int nearBerthId[203][203];
int nearBerthLength[203][203];