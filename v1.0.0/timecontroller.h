#ifndef TIMECONTROLLER_H
#define TIMECONTROLLER_H

#include <QWidget>
#include <QSpinBox>
#include <QSlider>
#include <QHBoxLayout>

class TimeController : public QWidget
{
    Q_OBJECT
public:
    explicit TimeController(QWidget *parent = nullptr);
    int getValue();
    QSpinBox* spin;
    QSlider* slider;

signals:

};

#endif // TIMECONTROLLER_H
