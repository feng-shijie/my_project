#ifndef MYSLIDER_H
#define MYSLIDER_H

//需要用到的函数要包含所有头文件
//重写进度条
#include <QWidget>
#include <QObject>
#include<QMouseEvent>
#include<QSlider>

class Myslider : public QSlider
{
    Q_OBJECT
public:
    explicit Myslider(QWidget *parent = nullptr);
    ~Myslider();

signals:
    void playersign(qint64 val);

public slots:
    void mousePressEvent(QMouseEvent *ev);

};

#endif // MYSLIDER_H
