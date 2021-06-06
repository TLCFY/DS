#include "student.h"
#include <QDebug>

Student::Student(): ID(0),currentStatus(wait),speed(1),source(0),dest(0),
                    phaseStart(0),phaseEnd(0),phaseTotalTime(0),phaseSpentTime(0),
                    pathTotalTime(0)
{
    isVisited = new bool[90]{false};
}

//获取学生的起点和目的地
void Student::setIndex(int sourceIndex,int destIndex)
{
    source = sourceIndex;
    dest = destIndex;
}

//获取用户选择的策略
void Student::setSTG(STG stg)
{
    strategy = stg;
}

//设置用户的状态
void Student::setStatus(Status status)
{
    currentStatus = status;
}

//返回学生现在所处状态
Status Student::retStatus()
{
    return currentStatus;
}

//返回学生图标应当刷新的位置
QPointF Student::retPos()
{
    return currentPos;
}

//返回路段是否允许骑行
bool Student::retCycling()
{
    return isCycling;
}

//返回用户策略
STG Student::retStg()
{
    return strategy;
}

//返回路径所需总时间
double Student::retTime()
{
    qDebug()<<"inner spent"<<pathTotalTime;
    return pathTotalTime;
}

//更新学生图标应当刷新的位置
void Student::updatePos(BuildGrp& builds, double spentTime)
{
    if(currentStatus == wait)
    {
        phaseStart = path.pop();
        phaseEnd = path.pop();
        currentPos = builds[phaseStart].pos;
        phaseTotalTime = phaseTime.pop();
        currentStatus = navigating;
    }
    else if(currentStatus == navigating)
    {
        phaseSpentTime += spentTime;
        //学生到达一段路线的终点
        if(phaseSpentTime >= phaseTotalTime)
        {
            //如果这段路线是路径的最后一段，则导航结束
            if(phaseEnd == dest)
            {
                currentPos = builds[dest].pos;
                currentStatus = arrived;
                phaseSpentTime = 0;
            }
            //如果只是途中的一个转折点，则更新路段到下一段
            else
            {
                qDebug()<<"current "<<currentPos<<"next "<<builds[phaseEnd].pos;
                phaseStart =phaseEnd;
                currentPos = builds[phaseEnd].pos;
                phaseEnd = path.pop();
                phaseSpentTime = 0;
                phaseTotalTime = phaseTime.pop();
                RoadPtr currentRoad = builds[phaseStart].firstRoad->nextRoad;
                while(currentRoad)
                {
                    if(currentRoad->nextIndex==phaseEnd)
                    {
                        if(currentRoad->isCycling)
                        {
                            isCycling=true;
                        }
                        else
                        {
                            isCycling = false;
                        }
                        break;
                    }
                    currentRoad = currentRoad->nextRoad;
                }
            }
        }
        //学生在路段中间
        else
        {
            float currentX = currentPos.x();
            float currentY = currentPos.y();
            float xDistance = builds[phaseEnd].pos.x() - builds[phaseStart].pos.x();
            float yDistance = builds[phaseEnd].pos.y() - builds[phaseStart].pos.y();
            float ratio = spentTime/phaseTotalTime;
            currentPos.setX(currentX+ratio*xDistance);
            currentPos.setY(currentY+ratio*yDistance);
           if(!isVisited[phaseStart])
            {
                qDebug()<<"from "<<builds[phaseStart].name<<" to "<<builds[phaseEnd].name<<"namely from "<<phaseStart<<" to "<<phaseEnd<<" expect time "<<phaseTotalTime;
                RoadPtr currentRoad = builds[phaseStart].firstRoad->nextRoad;
                while(currentRoad)
                {
                    if(currentRoad->nextIndex==phaseEnd)
                    {
                        qDebug()<<"distance "<<currentRoad->distance<<" crowdness is "<<currentRoad->crowdeness;
                        break;
                    }
                    currentRoad = currentRoad->nextRoad;
                }
                isVisited[phaseStart]=true;
            }

        }
    }
}

//获取路径
void Student::getPath(Map& map)
{
   int* prev = new int[map.maxIndex];
   double* time = new double[map.maxIndex];
   double* distance = new double[map.maxIndex];
   QVector<int> pass;
   int desttmp = dest;
   int sourcetmp = source;
   maxIndex = map.maxIndex;
   if(strategy==limitshortest)
   {
        while(!selected.isEmpty())
        {
            QMap<int, QString>::iterator itr = selected.begin();
            double minDis = INFINITY;
            while(itr!=selected.end())
            {
                dest = itr.key();
                Dijkstra(map,prev,distance,time);
                if(distance[dest]<minDis)
                {
                    dest = itr.key();
                }
                itr++;
            }
            pass.push_back(dest);
            selected.remove(dest);
        }
        dest = desttmp;
        QVector<int>::reverse_iterator rItr = pass.rbegin();
        while(rItr!=pass.rend())
        {
            source = *rItr;
            Dijkstra(map,prev,distance,time);
            pathTotalTime+=time[dest];
            reverse(prev,time);
            dest = source;
            rItr++;
        }
        source = sourcetmp;
        Dijkstra(map,prev,distance,time);
        pathTotalTime+=time[dest];
        reverse(prev,time);
        path.push(source);
        dest = desttmp;
   }
   else
   {
       //qDebug()<<"source "<<source<<" dest "<<dest;
       Dijkstra(map,prev,distance,time);
       //qDebug()<<"dijkstra finished";
       pathTotalTime=time[dest];
       reverse(prev,time);
       path.push(source);
   }
}

//Dijkstra获取最优路线
void Student::Dijkstra(Map& map,int* prev, double* distance, double* time)
{
    //初始化
    initialize(prev,distance,time);
    //qDebug()<<"here no bug,stg is"<<strategy;
    switch(strategy)
    {
    case shortest:
    case limitshortest:getShortest(map,prev,distance,time);break;
    case fastest:
    case trafficFastest:getLeastTime(map,prev,distance,time);break;
    default:;break;
    }
}

//初始化前驱数组、距离数组和时间数组
void Student::initialize(int* prev,double* distance,double* time)
{
    for(int i = 0; i < maxIndex; i++)
    {
        time[i]=INFINITY;
        distance[i] = INFINITY;
        prev[i] = -1;
    }
    time[source] = 0;
    distance[source] = 0;
}

//最短距离策略路线获取
void Student::getShortest(Map& map,int* prev,double* distance,double* time)
{
    bool* isFinal = new bool[maxIndex]{false};//一定要初始化，否则会导致无法更新最优值
    for(int i = 0; i < maxIndex; i++)
    {
        int minDisIndex = source;
        double minDistance = INFINITY;
        for(int i=0;i<maxIndex;i++)
        {
            if(distance[i]<minDistance&&!isFinal[i])
            {
                minDisIndex = i;
                minDistance = distance[i];
            }
        }
        isFinal[minDisIndex] = true;
        if(minDisIndex == dest)
        {
            return;
        }

        RoadPtr currentRoad = map.builds[minDisIndex].firstRoad->nextRoad;
        while(currentRoad)
        {
            int nextIndex = currentRoad->nextIndex;
            double distanceToNext = currentRoad->distance;
            double altDistance = distance[minDisIndex]+distanceToNext;
            double timeToNext = currentRoad->distance/(speed*(currentRoad->crowdeness));
            double altTime = time[minDisIndex]+timeToNext;
            if(altDistance < distance[nextIndex])
            {
                prev[nextIndex] = minDisIndex;
                distance[nextIndex] = altDistance;
                time[nextIndex] = altTime;
                //if(nextIndex==36||nextIndex==37||nextIndex==57||nextIndex==38||nextIndex==23)
                //qDebug()<<"shortest: "<<"time["<<nextIndex<<"] "<<" = "<<altTime;
            }
            currentRoad = currentRoad->nextRoad;
        }
    }
}

//最短时间策略路线获取
void Student::getLeastTime(Map& map,int* prev,double* distance,double* time)
{
    bool* isFinal = new bool[maxIndex]{false};
    for(int i = 0; i < maxIndex; i++)
    {
        int minTimeIndex = source;
        double minTime = INFINITY;
        for(int i=0;i<maxIndex;i++)
        {
            if(time[i]<minTime&&!isFinal[i])
            {
                minTimeIndex = i;
                minTime = time[i];
            }
        }
        isFinal[minTimeIndex] = true;
        if(minTimeIndex == dest)
        {
            return;
        }

        RoadPtr currentRoad = map.builds[minTimeIndex].firstRoad->nextRoad;
        while(currentRoad)
        {
            int nextIndex = currentRoad->nextIndex;
            double timeToNext = currentRoad->distance/((speed+(strategy==fastest&&currentRoad->isCycling?0:1))*(currentRoad->crowdeness));
            double altTime = time[minTimeIndex]+timeToNext;
            if(altTime<time[nextIndex])
            {
                prev[nextIndex] = minTimeIndex;
                time[nextIndex] = altTime;
                //if(nextIndex==36||nextIndex==37||nextIndex==57||nextIndex==38||nextIndex==23)
                //qDebug()<<"fastest: "<<"time["<<nextIndex<<"] "<<" = "<<altTime;
            }
            currentRoad = currentRoad->nextRoad;
        }
    }
}

//对Dijkstra中获取的反向路径、反向时间求正
void Student::reverse(int* prev,double* time)
{
    int currentIndex = dest;
    double currentTime = time[currentIndex];
    while(currentIndex != source)
    {
        path.push(currentIndex);
        //qDebug()<<"push "<<currentIndex;
        //qDebug()<<"current "<<currentIndex;
        currentIndex = prev[currentIndex];
        double prevTime = time[currentIndex];
        //qDebug()<<"from "<<currentIndex<<' '<<currentTime<<" to "<<prev[currentIndex]<<' '<<prevTime;
        phaseTime.push(currentTime - prevTime);
        //qDebug()<<"time: "<<currentTime-prevTime;
        currentTime = prevTime;
    }
    //path.push(currentIndex);
}
