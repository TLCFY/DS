#ifndef STUDENT_H
#define STUDENT_H

#include <QPointF>
#include <QStack>
#include <QMap>
#include "map.h"

enum Status{wait = 0, navigating, arrived};

class Student
{
private:
    bool* isVisited;/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool isCycling =false;
    int ID;
    Status currentStatus;
    double speed;
    int maxIndex;
    int source;             //路径起点
    int dest;               //路径终点
    STG strategy;
    QStack<int> path;       //学生移动路径
    QStack<double> phaseTime;//对应路径的各段所用时间
    QPointF currentPos;     //学生移动过程中的位置
    int phaseStart;         //学生所在路段的起点index
    int phaseEnd;           //学生所在路段的终点index
    double phaseTotalTime;  //学生所在路段所需的总时间
    double phaseSpentTime;  //学生在路段上已走时间
    double pathTotalTime;   //路径所需总时间

public:
    QMap<int,QString> selected;
    Student();
    bool retCycling();
    STG retStg();
    double retTime();
    void setStatus(Status);
    void setIndex(int,int);     //获取学生的起点和目的地
    void setSTG(STG);           //获取用户选择的策略
    Status retStatus();         //返回学生现在所处状态
    QPointF retPos();           //返回学生图标应当刷新的位置
    void updatePos(BuildGrp&,double); //更新学生图标应当刷新的位置
    void getPath(Map&); //获取路径
    void Dijkstra(Map& map,int* prev,double* distance,double* time);    //Dijkstra获取最优路线
    void initialize(int* prev,double* distance,double* time);  //初始化前驱数组、距离数组和时间数组
    void getShortest(Map& map,int* prev,double* distance,double* time); //最短距离策略路线获取
    void getLeastTime(Map& map,int* prev,double* distance,double* time);//最短时间策略路线获取
    void reverse(int* prev,double* time); //对Dijkstra中获取的反向路径、反向时间求正
};

#endif // STUDENT_H
