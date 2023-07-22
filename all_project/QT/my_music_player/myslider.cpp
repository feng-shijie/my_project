#include "myslider.h"
#include<QDebug>

Myslider::Myslider(QWidget *parent) : QSlider(parent)
{

}

Myslider::~Myslider()
{

}

//重写鼠标点击事件
void Myslider::mousePressEvent(QMouseEvent *ev)
{
    //调用父类的鼠标点击处理事件,这样可以不影响其它情况
    QSlider::mousePressEvent(ev);

    //获取鼠标点击位置信息
    int current;    //获取鼠标点击位置
    double per;     //获取占总长度的百分比
    qint64 val;     //鼠标点击位置的值

    //判断QSlider是纵向还是横向
    if(this->orientation() != Qt::Horizontal)
    {
        current = ev->pos().y();
        per = current*1.0 / this->height();
        val = this->maximum() - this->maximum() * per;
    }
    else
    {
        current = ev->pos().x();
        per = current*1.0 / this->width();
        val = this->maximum() * per;
    }

    //发射信号
    emit playersign(val);
}
