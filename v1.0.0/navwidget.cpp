//建立子窗口
//
#include "navwidget.h"
#include <QDebug>

NavWidget::NavWidget(QWidget *parent) : QWidget(parent)
{
    back.load(":/Resource/newCam_Line");
    this->setFixedSize(914,579);

    navTimer = new QTimer;
    navTimer->setInterval(49);
    p = (Widget*)this->parent();

    isStart = false;

    connect(p,SIGNAL(startNav()),this,SLOT(myStart()));
    connect(navTimer,SIGNAL(timeout()),this,SLOT(update()));
}

void NavWidget::myStart(void)
{
    navTimer->start();
    user.getPath(navMap);
    user.setStatus(wait);
    qDebug()<<"path got";
    isStart = true;
    qDebug()<<"start";
    emit displaySig();
}

void NavWidget::paintEvent(QPaintEvent* event)
{
    QPainter backPainter(this);
    backPainter.drawPixmap(0,0,back);

    if(isStart)
    {
        user.updatePos(navMap.builds,(p->timeRatio));
        QPointF pos = user.retPos();
        QPainter studentPainter(this);
        if(user.retCycling()&&user.retStg()==trafficFastest)
        {
            student.load(":/Resource/bike");
        }
        else
        {
            student.load(":/Resource/icon");
        }
        student = student.scaled(30,30);//重置大小
        studentPainter.drawPixmap(pos.x()-15,pos.y()-25,student);
    }
}

