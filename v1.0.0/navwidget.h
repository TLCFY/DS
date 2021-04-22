#ifndef NAVWIDGET_H
#define NAVWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QVector>
#include <QTimer>
#include <QStack>
#include <QPainter>
#include <QPen>
#include <QDateTime>
#include "map.h"
#include "widget.h"
#include "student.h"

class NavWidget : public QWidget
{
    Q_OBJECT
public:
    Student user;
    Map navMap;
    explicit NavWidget(QWidget *parent = nullptr);
    void setSpentTime();

protected:
    void paintEvent(QPaintEvent* event);

private:
    bool isStart;
    Widget *p;
    QPixmap back;
    QPixmap student;
    QTimer* navTimer;
    double spentTime;
public slots:
    void myStart(void);

signals:
    void displaySig();
};

#endif // NAVWIDGET_H
