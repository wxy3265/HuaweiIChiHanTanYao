//
// Created by wxy3265 on 2024/3/8.
//

#include "tanyao.h"
char maze[300][300];
int frame, totalMoney, lastFrame, framesum;
int capacity;
const bool cerrSwitch = false; //调试信息总开关
const bool cerrBerth = true; //港口调试信息
const bool cerrRobot = true; //机器人调试信息
const bool cerrTotalGetValue = true; //总获取金额调试信息
const bool cerrShip = false; //船舶调试信息
const bool cerrFrame = true; //帧标记调试信息
bool avoidCrash = false;
int PathAlgorithm = 1; //0为BFS 1为A*
int mostBerthNumber = 100000;//可以连续到达的港口数量
int mapNUM = 0;
const bool bindBerth = false; //机器人与港口绑定为true
const double deltaLength = 2.5;
const double deltaTime = 0;
const double deltaTarget = 0;
bool berthOpen[10] = {true, true, true, true, true, true, true, true, true, true};
bool berthOpenforShip[10] = {true, true, true, true, true, true, true, true, true, true};
int nearBerthId[203][203];
int nearBerthLength[203][203];
int robotHome[10];
bool robotFirstMission[10];
int berthRobot[10];
int setedShipMissionTimes[10];
int nearRobotId[200000];
int nearRobotDis[200000];
int pathLengthToBerth[10][203][203];
int pathLengthToStart[10][203][203];
int berth_to_berth[10][10];
int pathLength[10][203][203];
bool visitGoods[200007];