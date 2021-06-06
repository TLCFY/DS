#include "timecontroller.h"

TimeController::TimeController(QWidget *parent) : QWidget(parent)
{
    spin = new QSpinBox(this);
    slider = new QSlider(Qt::Horizontal,this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(spin);
    layout->addWidget(slider);

    connect(spin,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),slider,&QSlider::setValue);
    connect(slider,&QSlider::valueChanged,spin,&QSpinBox::setValue);
    spin->setMinimum(100);
    spin->setMaximum(2000);

    slider->setMinimum(100);
    slider->setMaximum(2000);

}

int TimeController::getValue()
{
    return slider->value()/20;
}
