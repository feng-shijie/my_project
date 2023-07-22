#include "item_widget.h"
#include"widget.h"
#include<stdio.h>
#include<QDebug>

#define BUTTON_WIDTH 40

item_widget::item_widget(QWidget *parent) : QWidget(parent)
{
    QFont fn("宋体",10,87);
    p_start= false;     //设置暂停与开始的开关
    love = false;       //设置收藏的开关

    //我喜欢
    like_it_Button = new QPushButton(this);
    //需要在按钮定义的地方使用setObjectName给对象命名这样才能通过findChild查找到指定对象
    like_it_Button->setObjectName("like_it_Button");
    like_it_Button->setFont(fn);
    like_it_Button->setText("收藏");
    like_it_Button->resize(this->height()*2,this->height());
    like_it_Button->move(0,this->height());

    //暂停and开始
    Pause_start_Button = new QPushButton(this);
    //需要在按钮定义的地方使用setObjectName给对象命名这样才能通过findChild查找到指定对象
    Pause_start_Button->setObjectName("Pause_start_Button");
    Pause_start_Button->setFont(fn);
    Pause_start_Button->setText("暂停");
    Pause_start_Button->resize(this->height()*2,this->height());
    //Pause_start_Button->move(this->height()*32,this->height());
    Pause_start_Button->move(this->height()*3,this->height());
    //Pause_start_Button->hide();

    //歌名
    label = new QLabel(this);
    label->setFont(fn);
    label->resize(1000,this->height());
    label->move(this->height()*6,this->height());
    label->lower();  //显示在最下方

    this->setMouseTracking(true);       //实时追踪鼠标
    label->setMouseTracking(true);

    connect(Pause_start_Button,&QPushButton::pressed,this,&item_widget::slotpressed);
    connect(like_it_Button,&QPushButton::pressed,this,&item_widget::slotpressed);

}

//获取widget指定的初始值
void item_widget::setindex(int id, My_listwidget name, QString songname)
{
    this->id = id;
    this->list_name = name;
    label->setText(songname);

}

//定义是否为收藏的
void item_widget::setlove(bool lv)
{
    if(lv)    this->like_it_Button->setText("❤");
    else    this->like_it_Button->setText("收藏");
}

int item_widget::getindex()
{
    return id;
}

//按钮信号
void item_widget::slotpressed()
{
    QObject *sender = QObject::sender();

    if(sender == Pause_start_Button)
    {
        if(p_start == false && Pause_start_Button->text() != "暂停")
            p_start = true;

        if(p_start == false)
        {
            Pause_start_Button->setText("开始");
            p_start = true;
        }
        else
        {
            Pause_start_Button->setText("暂停");
            p_start = false;
        }

        emit this->item_widget_index(id,list_name,label->text());
    }

    if(sender == like_it_Button)
    {
        //第一个点击的列表要加入收藏表,其他列表要修改图标,修改图标却无法修改love值,所以判断一下,
        //如果只修改了图标没有修改值,那说明该列表只是同一收藏数据,并未真正点击过,那这一次的点击就是要撤销收藏
        if(love == false && like_it_Button->text() != "收藏")
            love = true;
        else if(love == true && like_it_Button->text() != "❤")
            love = false;

        if(love == false)
        {
            like_it_Button->setText("❤");
            love = true;
        }
        else
        {
            like_it_Button->setText("收藏");
            love = false;
        }
        emit this->item_widget_love(id,list_name,label->text(),love);
    }
}

void item_widget::enterEvent(QEvent *)      //鼠标进入
{
    //Pause_start_Button->show();
}
void item_widget::leaveEvent(QEvent *)      //鼠标移出
{
    //Pause_start_Button->hide();
}
