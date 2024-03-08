//#include "tanyao.h"
#include "Map.h"

int robotHome[12];
int berthRobot[12];
bool visitGoods[200007];
struct Operation{
    Goods targetGoods;
    Berth targetBerth;
    int totalDistance;
    double efficiency;
    bool operator<(const Operation &x) const{
        return efficiency < x.efficiency;
    }
};
priority_queue<Operation>operation[12];

struct Distance_to_berth{
    int berthId;
    int robotId;
    int distance;
}to_berth_distance[107];

bool cmp(Distance_to_berth x, Distance_to_berth y){
    return x.distance < y.distance;
}

void allocateHome(){
    for(int i = 0; i <= 9; i++)robotHome[i] = berthRobot[i] = -1;
    int cnt = 0;
    for(int i = 0; i <= 9; i++){
        for(int j = 0; j <= 9; j++){
            to_berth_distance[++cnt].robotId = i;
            to_berth_distance[cnt].berthId = j;
            to_berth_distance[cnt].distance = Path(robot[i].position, berth[j].position, 1).length;
        }
    }
    sort(to_berth_distance + 1,to_berth_distance + cnt + 1,cmp);
    for(int i = 0; i <= cnt; i++){
        if(robotHome[to_berth_distance[i].robotId] != -1)continue;
        if(berthRobot[to_berth_distance[i].berthId] != -1)continue;
        robotHome[to_berth_distance[i].robotId] = to_berth_distance[i].berthId;
        berthRobot[to_berth_distance[i].berthId] = to_berth_distance[i].robotId;
    }
}

void calcEfficiency(int start){
    for(int i = 0; i < newGoods.size(); i++) {
        int dis = Path(newGoods[i].position, berth[start].position, 0).length;
        double efficiency = 1.0 * newGoods[i].value / (dis * 2.0);
        operation[start].push((Operation){newGoods[i], berth[start], dis * 2, efficiency});
    }
}

int _main() {
    Map::init();
    allocateHome();
    while (true){
        Map::update();
        for(int i = 0; i <= 9; i++)
            calcEfficiency(i);
        while(newGoods.size())newGoods.pop_back();
        for(int i = 0; i <= 9; i++){
            if(robot[i].getState() == RobotState::FREE){
                while(1){
                    if (visitGoods[operation[robotHome[i]].top().targetGoods.id] == 0)break;
                    operation[robotHome[i]].pop();
                }
                while(1){
                    int dis = operation[robotHome[i]].top().totalDistance / 2;
                    int time = operation[robotHome[i]].top().targetGoods.time;
                    if(frame + dis < time + 1000)break;
                    operation[robotHome[i]].pop();
                }
                robot[i].setMission(operation[robotHome[i]].top().targetGoods, operation[robotHome[i]].top().targetBerth);
                visitGoods[operation[robotHome[i]].top().targetGoods.id] = 1;
                operation[robotHome[i]].pop();
            }
        }
    }
    return 0;
}
