//建立逻辑图
#include "map.h"
#include <QDebug>

//Map类创建时即调用createMap创建图
Map::Map()
{
    maxIndex = 0;
    createMap();
}

Map::~Map()
{

}

//读取文件，创建与地图相对应的图
void Map::createMap()
{
    //创建文件操作相关变量
    QFile vex(":/Resource/Vex");
    QFile edge(":/Resource/Edge");
    vex.open(QIODevice::ReadOnly);
    edge.open(QIODevice::ReadOnly);
    QTextStream vexStream(&vex);
    QTextStream edgeStream(&edge);

    //初始化顶点表
    while(!vexStream.atEnd())
    {
        QString name;
        char buf = 0;
        int index = 0;
        int x = 0,y = 0;
        vexStream>>name>>index>>x>>buf>>y;
        //qDebug()<<"name "<<name<<'\t'<<index<<'\t'<<x<<','<<y;
        builds[index].name = name;
        builds[index].isBuild = name.contains(QRegExp("[\\x4e00-\\9fa5]+"));//正则表达式判断名称中是否存在中文以区分建筑顶点和道路顶点
        builds[index].pos.setX(x);
        builds[index].pos.setY(y);
        builds[index].firstRoad = new Road;
        builds[index].firstRoad->nextRoad = NULL;
        builds[index].firstRoad->crowdeness = 0;
        builds[index].firstRoad->distance = 0;
        builds[index].firstRoad->nextIndex = 0;
        builds[index].firstRoad->nextRoad = 0;
        maxIndex = (index>maxIndex?index:maxIndex);
    }

    //构建边表
    while(!edgeStream.atEnd())
    {
        QString nameBuffer;
        int sourceIndex = 0, destIndex = 0;
        double distance = 0;
        //获取相关信息
        edgeStream >> nameBuffer >> nameBuffer;//丢弃两点的名称
        edgeStream >> sourceIndex >> destIndex >> distance;
        //qDebug()<<sourceIndex<<' '<<destIndex<<' '<<distance;
        connectToVex(sourceIndex,destIndex,distance);
    }
    for(int i =0;i<maxIndex;i++)
    {
        RoadPtr currentPtr = builds[i].firstRoad->nextRoad;
        while(currentPtr)
        {
            //qDebug()<<"for "<< builds[i].name<<' '<<builds[i].pos;
            //qDebug()<<"next "<<builds[currentPtr->nextIndex].name<<' '<<builds[currentPtr->nextIndex].pos;
            //qDebug()<<"crowdness"<<currentPtr->crowdeness;
            //qDebug()<<"distance "<<currentPtr->distance;
            currentPtr = currentPtr->nextRoad;
        }
    }
}

//根据源点、端点、距离添加两点之间的边
void Map::connectToVex(int sourceIndex, int destIndex, double distance)
{

    //寻找新边的添加位置5
    RoadPtr currentSourceRoad = builds[sourceIndex].firstRoad;
    RoadPtr currentDestRoad = builds[destIndex].firstRoad;
    while(currentSourceRoad->nextRoad)
    {
        currentSourceRoad = currentSourceRoad->nextRoad;
    }
    while(currentDestRoad->nextRoad)
    {
        currentDestRoad = currentDestRoad->nextRoad;
    }

    //双向添加新边
    RoadPtr newSourceRoad = new Road;
    RoadPtr newDestRoad = new Road;
    newSourceRoad->nextIndex = destIndex;
    newDestRoad->nextIndex = sourceIndex;
    newSourceRoad->distance = distance;
    newDestRoad->distance = distance;
    //newSourceRoad->crowdeness = 1;
    //newDestRoad->crowdeness = 1;
    newSourceRoad->crowdeness=QRandomGenerator::global()->bounded(0.90)+0.1;
    newDestRoad->crowdeness=QRandomGenerator::global()->bounded(0.90)+0.1;
    if(sourceIndex == 38&&(destIndex==39||destIndex==84))
    {
        newSourceRoad->isCycling=true;
        newDestRoad->isCycling=true;
    }
    else
    {
        newSourceRoad->isCycling=false;
        newDestRoad->isCycling=false;
    }
    newSourceRoad->nextRoad = NULL;
    newDestRoad->nextRoad = NULL;
    currentSourceRoad->nextRoad = newSourceRoad;
    currentDestRoad->nextRoad = newDestRoad;
}

