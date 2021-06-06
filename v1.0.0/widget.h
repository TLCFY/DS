#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTime>
#include <QFile>
#include <QCompleter>
#include <QTextStream>
#include <QMouseEvent>
#include <QtMath>
#include <QVector>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void setSearchBox(void);    //设置检索框
    void getInfo(void);         //获取出发点、目的地和策略信息
    void myConnect(void);       //设置按钮点击事件响应
    int timeRatio;
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent* event);

public slots:
    //发送信号让导航窗口开始导航绘图
    void mystart(void);
    //计算时间流动
    void timeFlow(void);
    //检验用户是否需要点选途径建筑
    void checkStrategy(void);
    //展示时间
    void displayTime(void);
    //获取时间比例
    void getTimeRatio();

signals:
    //导航开始信号
    void startNav(void);

private:
    QTime startTime;
    QTime pathTime;
    QTime endTime;
    QTimer* widgetTimer;
    bool isSelect=false;
    QMap<int,QString> selected;
    Ui::Widget *ui;
};
#endif // WIDGET_H
