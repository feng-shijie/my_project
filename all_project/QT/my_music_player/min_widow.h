#ifndef MIN_WIDOW_H
#define MIN_WIDOW_H

//重写小界面的widget
#include <QWidget>
#include<QPushButton>
#include<QMouseEvent>
#include<QEvent>
#include<QPalette>
#include"label_scroll.h"

class Min_widow : public QWidget
{
    Q_OBJECT
public:
    explicit Min_widow(QWidget *parent = nullptr);
    ~Min_widow();

    void Initialization();  //初始化
    void slotClose();       //关闭
    void slotReduction();   //还原
    void slotLast_song();   //上一首
    void slotnext_song();   //下一首
    void slotstart_stop();  //开始和暂停

    void setname(QString name);     //获取widget的歌曲名

    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);

    void enterEvent(QEvent *);      //鼠标进入
    void leaveEvent(QEvent *);      //鼠标移出

signals:
    void SingReduction();   //还原信号
    void singlast();        //上一首
    void singnext();        //下一首
    void singstart();       //暂停和开始

private:
    QPushButton *Close;         //关闭
    QPushButton *Reduction;     //还原
    QPushButton *Last_song;     //上一首
    QPushButton *next_song;     //下一首
    QPushButton *start_stop;    //暂停和开始
    QPushButton *love;          //加入我的收藏
    label_scroll *label;        //歌名

    QPoint leftval;            //左键点击值

};

#endif // MIN_WIDOW_H
