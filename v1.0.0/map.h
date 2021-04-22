#ifndef MAP_H
#define MAP_H

#include <QPoint>
#include <QTextStream>
#include <QFile>
#include <QTime>
#include <QRandomGenerator>

#define MAXNUM 300
enum STG{shortest = 0, fastest, limitshortest, trafficFastest};

//道路抽象为边结点
typedef struct edge{
    int nextIndex;
    double distance;
    bool isCycling;
    double crowdeness;
    struct edge* nextRoad;
}Road,*RoadPtr;

//建筑抽象为顶点
typedef struct vertex{
    bool isBuild;   //用于区分建筑顶点和道路顶点
    QString name;
    QPoint pos;
    RoadPtr firstRoad;
}Build,BuildGrp[MAXNUM];

class Map
{
private:

protected:

public:
    BuildGrp builds;             //定点列表，存储地图上所有建筑
    double Matrix[MAXNUM][MAXNUM]; //权重矩阵，用于Dijkstra算法
    int maxIndex;

    //读取文件，创建与地图相对应的图
    void createMap();
    //根据strategy的类型决定矩阵中的权值
    void initMatrix(STG strategy);
    //根据源点、端点、距离添加两点之间的边
    void connectToVex(int sourceIndex, int destIndex, double distance);

    //Map类创建时即调用createMap创建图
    Map();
    ~Map();
};

#endif // MAP_H

