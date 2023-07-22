#ifndef LABEL_SCROLL_H
#define LABEL_SCROLL_H

//需要用到的函数要包含所有头文件
//重写label(滚动字幕)
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QTimerEvent>
#include <QPaintEvent>
#include <QTextDocument> //判断富文本用的
#include <QPainter>

class label_scroll : public QLabel
{
    Q_OBJECT
public:
    explicit label_scroll(QWidget *parent = nullptr);
    ~label_scroll();

public slots:
    //把Qlabel这个部件的settext属性重新定义
    void setText(const QString &txt);

    //判断是否启用滚动
    void upateLabelRollingState();

    //重绘事件，根据位移量left显示文本
    void paintEvent(QPaintEvent *e);
    //定时修改位移量，到末尾改为开头 负责修改当前像素位移值
    void timerEvent(QTimerEvent *e);
    //根据给定的数值，修改滚动的速度，sp是一次滚动多少像素，st是多少秒触发一次滚动
    void setspeed(int sp = 10, int st = 300);
private:

    int left;           //标明当前的像素滚动量

    int timerID;        //定时器id
    int text_wpixel;    //存储当前label内存字符串的像素水平长度

    int speedt;     //多久触发一次滚动
    int spixel;     //一次滚动多少像素

    QString blank;  //空格
    //QString text;
    int blank_wp;   //空格的像素宽度

    uint8_t flag;   //判断是否应该开启滚动 0否 1真

signals:

};

#endif // LABEL_SCROLL_H
