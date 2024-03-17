//
// Created by wxy3265 on 2024/3/16.
//
#include "Map.h"
/*
void calcEfficiency(int startBerthId);
void calcEfficiencyLarge(int startBerthId);
void calcEfficiencyPlus(int robId);
void calcEfficiencyUltra(int startBerthId);
void shipGetMission(int shipId);
void shipGetMissionSE(int shipId);
void shipGetMissionSEPlus(int shipId);
void shipGetMissionMiniPlus(int shipId);
void shipGetMissionMini(int shipId);
void shipGetMissionPlus(int shipId);

void calcEfficiency(int startBerthId) {
    for(auto & newGood : newGoods) {
        int nearBerthId = Map::getNearBerthId(newGood.position);
        double pathLength = Map::getLengthFromBerthToPoint(startBerthId, newGood.position) * 2;
        double efficiency = 1.0 * newGood.value / pathLength;
//        if (newGood.value > 150)
        operation[startBerthId].push((Operation){newGood, startBerthId, pathLength, pathLength / 2.0, efficiency});
    }
}
void calcEfficiencyLarge(int startBerthId) {
    for(auto & newGood : newGoods) {
        if (newGood.value < 100) continue;
        int nearBerthId = Map::getNearBerthId(newGood.position);
        double pathLength = Map::getLengthFromBerthToPoint(startBerthId, newGood.position) * 2;
        double efficiency = 1.0 * 20000 / pathLength;
//        if (newGood.value > 150)
        operation[startBerthId].push((Operation){newGood, startBerthId, pathLength, pathLength / 2.0, efficiency});
    }
}
void checkOperation(int berthId) {
    cerr << "berthID:" <<berthId << "\n";
    priority_queue<Operation> q;
    while (!operation[berthId].empty()) {
        q.push(operation[berthId].top());
        cerr << "totaldistance:" <<operation[berthId].top().totalDistance << "\n";
        cerr << "efficiency:" << operation[berthId].top().efficiency << "\n";
        cerr << "targetGoods.value:" << operation[berthId].top().targetGoods.value << "\n";
        operation[berthId].pop();
    }
    while (!q.empty()) {
        operation[berthId].push(q.top());
        q.pop();
    }
    cerr << "\n";
}
void calcEfficiencyUltra(int startBerthId) {
    for(auto & newGood : newGoods) {
        int nearBerthId = Map::getNearBerthId(newGood.position);
        if(nearBerthId != startBerthId) continue;
        double pathLength = Map::getLengthFromBerthToPoint(startBerthId, newGood.position) * 2;
        double efficiency = 1.0 * newGood.value / pathLength;
//        if (newGood.value > 150)
        operation[nearBerthId].push((Operation){newGood, nearBerthId, pathLength, pathLength / 2.0, efficiency});
    }
}
void calcEfficiencyPlus(int robId){
//    cerr << "begin";
    for(auto & newGood : newGoods) {
//        if (newGood.value < 150) continue;
        int nearBerthId = Map::getNearBerthId(newGood.position);
        int nowBerthId;
        if (robotFirstMission[robId]) nowBerthId = robotHome[robId];
        else nowBerthId = robot[robId].getTargetId();
        if (nowBerthId == -1) continue;
        double pathLength = Map::getLengthFromBerthToPoint(nowBerthId, newGood.position) +
                            Map::getLengthFromBerthToPoint(nearBerthId, newGood.position) +
                            berth[nearBerthId].distance * 0;
        double goodsDistance = Map::getLengthFromBerthToPoint(nowBerthId, newGood.position);
//        double efficiency = 1.0 * 200 / pathLength;
        double efficiency = (double) newGood.value / (double) pathLength;
//        if (newGood.value > 150)
        cerr << "nowBerthID:" << nowBerthId << '\n';
        operation[nowBerthId].push((Operation){newGood, nearBerthId, pathLength, goodsDistance, efficiency});
    }
//    cerr << "end\n";
}
void shipGetMission(int shipId){
    double efficiency = -1000000;
    int targetBerth = -1;
    for(int i = 0; i <= 9; i++){
        if(visitBerth[i])continue;
        int num = berth[i].getGoodsNum();
        int k = 0;
        while(k * capacity < num){
            k++;
            int minum = min(k * capacity, num);
            double eff = 1.0 * berth[i].getHeadGoodsValue(minum) / (berth[i].distance * 2.0 * k);
            if(eff > efficiency){
                efficiency = eff;
                targetBerth = i;
            }
        }
    }
    if(targetBerth != -1){
        ship[shipId].setMission(ShipMission(targetBerth, -1));
        visitBerth[targetBerth] = true;
    }
}
void shipGetMissionSE(int shipId) {
    ship[shipId].setMission((ShipMission){shipId * 2, -1});
    ship[shipId].setMission((ShipMission){shipId * 2 + 1, -1});
}
void shipGetMissionSEPlus(int shipId) {
    setedShipMissionTimes[shipId]++;
    if (setedShipMissionTimes[shipId] % 2 == 0) {
        ship[shipId].setMission((ShipMission){shipId * 2, min(setedShipMissionTimes[shipId] * 5, capacity)});
    }
    else {
        ship[shipId].setMission((ShipMission){shipId * 2 + 1, min(setedShipMissionTimes[shipId] * 5, capacity)});
    }
}
void shipGetMissionMini(int shipId) {
    for (int i = shipId * 2; i < 10; i++) {
        ship[shipId].setMission(ShipMission{i, -1});
    }
    for (int i = 0; i < shipId * 2; i++) {
        ship[shipId].setMission(ShipMission{i, -1});
    }
}
void shipGetMissionMiniPlus(int shipId) {
    double efficiency = -1000000;
    int targetBerth1 = -1,targetBerth2 = -1;
    int cap1 = 0,cap2 = 0;
    for(int i = 0; i <= 9; i++){
        int num = berth[i].getGoodsNum();
        int minum = min(num, capacity);
        double eff = 1.0 * minum / (berth[i].distance * 2.0);
        if(eff > efficiency){
            efficiency = eff;
            targetBerth1 = i;
            cap1 = minum;
        }
    }

    for (int i = 0; i <= 9; i++) {
        int num1 = berth[i].getGoodsNum();
        for(int j = 0; j <= 9; j++){
            if(i == j)continue;
            int num2 = berth[j].getGoodsNum();
            for (int cnt1 = 0; cnt1 <= min(num1, capacity); cnt1++) {
                int cnt2 = min(capacity - cnt1, num2);
                double eff = 1.0 * (cnt1 + cnt2) / (berth[i].distance + berth[j].distance + 500.0);
                if(eff > efficiency){
                    if(cnt1 == 0){
                        efficiency = eff;
                        targetBerth1 = i;
                        cap1 = cnt1;
                    }else if(cnt2 == 0){
                        efficiency = eff;
                        targetBerth1 = j;
                        cap1 = cnt2;
                    }else{
                        efficiency = eff;
                        targetBerth1 = i;
                        targetBerth2 = j;
                        cap1 = cnt1;
                        cap2 = cnt2;
                    }
                }
            }
        }
    }
    if(targetBerth1 != -1){
        if(targetBerth2 == -1){
            ship[shipId].setMission(ShipMission(targetBerth1,-1));
            berth[targetBerth1].visitGoodsNumber += cap1;
            //visitGoodsNumber[targetBerth1] = true;
        }
        else{
            ship[shipId].setMission(ShipMission(targetBerth1,-1));
            ship[shipId].setMission(ShipMission(targetBerth2,-1));
            berth[targetBerth1].visitGoodsNumber += cap1;
            berth[targetBerth2].visitGoodsNumber += cap2;
        }
    }
}
void shipGetMissionPlus(int shipId){
    double efficiency = -1000000;
    int targetBerth1 = -1,targetBerth2 = -1;
    int cap1 = 0,cap2 = 0;
    for(int i = 0; i <= 9; i++){
        int num = berth[i].getGoodsNum();
        int minum = min(num, capacity);
        double eff = 1.0 * berth [i].getHeadGoodsValue(minum) / (berth[i].distance * 2.0);
        if(eff > efficiency){
            efficiency = eff;
            targetBerth1 = i;
            cap1 = minum;
        }
    }

    for (int i = 0; i <= 9; i++) {
        int num1 = berth[i].getGoodsNum();
        for(int j = 0; j <= 9; j++){
            if(i == j)continue;
            int num2 = berth[j].getGoodsNum();
            for (int cnt1 = 0; cnt1 <= min(num1, capacity); cnt1++) {
                int cnt2 = min(capacity - cnt1, num2);
                double eff = 1.0 * (berth[i].getHeadGoodsValue(cnt1) + berth[j].getHeadGoodsValue(cnt2)) / (berth[i].distance + berth[j].distance + 500.0);
                if(eff > efficiency){
                    if(cnt1 == 0){
                        efficiency = eff;
                        targetBerth1 = i;
                        cap1 = cnt1;
                    }else if(cnt2 == 0){
                        efficiency = eff;
                        targetBerth1 = j;
                        cap1 = cnt2;
                    }else{
                        efficiency = eff;
                        targetBerth1 = i;
                        targetBerth2 = j;
                        cap1 = cnt1;
                        cap2 = cnt2;
                    }
                }
            }
        }
    }
    if(targetBerth1 != -1){
        if(targetBerth2 == -1){
            ship[shipId].setMission(ShipMission(targetBerth1,-1));
            //berth[targetBerth1].visitGoodsNumber = min(cap1 + berth[targetBerth1].visitGoodsNumber,;
            visitBerth[targetBerth1] = true;
        } else {
            ship[shipId].setMission(ShipMission(targetBerth1,-1));
            ship[shipId].setMission(ShipMission(targetBerth2,-1));
            //berth[targetBerth1].visitGoodsNumber += cap1;
            //berth[targetBerth2].visitGoodsNumber += cap2;
            visitBerth[targetBerth1] = true;
            visitBerth[targetBerth2] = true;
        }
    }
}
*/