//建立主窗口
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include "timecontroller.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    widgetTimer = new QTimer(this);
    widgetTimer->setInterval(50);
    widgetTimer->start();
    //widgetTimer->setSingleShot(true);
    setSearchBox();
    myConnect();
}

//设置按钮点击事件响应
void Widget::myConnect(void)
{
    connect(ui->start,SIGNAL(clicked()),this,SLOT(mystart()));
    connect(widgetTimer,SIGNAL(timeout()),this,SLOT(timeFlow()));
    connect(widgetTimer,SIGNAL(timeout()),this,SLOT(checkStrategy()));
    connect(ui->map,SIGNAL(displaySig()),this,SLOT(displayTime()));
    connect(widgetTimer,SIGNAL(timeout()),this,SLOT(getTimeRatio()));
}

//设置检索框
void Widget::setSearchBox(void)
{
    //从文件中获取名称并存入wordList中
    QFile file(":/Resource/BuildsName");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString s = in.readAll();
    QStringList wordList;
    wordList = s.split("\r\n");

    //将Completer与Combo Box组合，实现查找的功能
    QCompleter* sourceCompleter = new QCompleter(wordList,this);
    QCompleter* destCompleter = new QCompleter(wordList,this);
    ui->sourceBox->addItems(wordList);
    ui->destBox->addItems(wordList);
    ui->sourceBox->setEditable(true);
    ui->destBox->setEditable(true);
    ui->sourceBox->setCompleter(sourceCompleter);
    ui->destBox->setCompleter(destCompleter);
}

//从检索框中获取出发点和目的地
void Widget::getInfo(void)
{
    STG stg;
    switch(ui->stg->currentIndex())
    {
    case 0:stg = shortest;break;
    case 1:stg=limitshortest;break;
    case 2:stg = fastest;break;
    case 3:stg = trafficFastest;break;
    }
    qDebug()<<"stg "<<stg;
    int source = ui->sourceBox->currentIndex();
    int dest = ui->destBox->currentIndex();
    ui->map->user.setIndex(source,dest);
    ui->map->user.setSTG(stg);
}

//检验用户是否需要点选途径建筑
void Widget::checkStrategy(void)
{
    //qDebug()<<"check";
    if(ui->stg->currentIndex()==1)
    {
        //qDebug()<<"check yes";
        isSelect = true;
    }
    else
    {
        isSelect = false;
    }
}

//获取时间比例
void Widget::getTimeRatio()
{
    timeRatio = ui->timeControl->getValue();
}

void Widget::mousePressEvent(QMouseEvent* event)
{
    if(isSelect)
    {
        QPointF pos = event->localPos();
        for(int i =0;i<ui->map->navMap.maxIndex;i++)
        {
            float xDif = pos.x()-ui->map->navMap.builds[i].pos.x();
            float yDif = pos.y()-ui->map->navMap.builds[i].pos.y();
            float distance = sqrt((pow(xDif,2)+pow(yDif,2)));
            if(distance < 3)
            {
                qDebug()<<"select "<<ui->map->navMap.builds[i].name<<' '<<pos.x()<<' '<<pos.y();
                ui->map->user.selected.insert(i,ui->map->navMap.builds[i].name);
                ui->selected->append(ui->map->navMap.builds[i].name);
            }
        }

    }
}

void Widget::displayTime(void)
{
    endTime = startTime = pathTime = QTime::currentTime();
    endTime = startTime.addSecs(ui->map->user.retTime());
    qDebug()<<endTime;
    ui->endTime->setText(endTime.toString("HH:mm:ss"));
}

//发送信号让导航窗口开始导航绘图
void Widget::mystart(void)
{
    getInfo();
    emit startNav();
}

//计算时间流动
void Widget::timeFlow(void)
{
    pathTime = pathTime.addSecs(timeRatio);
    if(pathTime >= endTime)
    {
        pathTime = endTime;
    }
    ui->startTime->setText(startTime.toString("HH:mm:ss"));
    ui->pathTime->setText(pathTime.toString("HH:mm:ss"));
    ui->endTime->setText(endTime.toString("HH:mm:ss"));
}

Widget::~Widget()
{
    delete ui;
}
