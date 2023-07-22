#ifndef ITEM_WIDGET_H
#define ITEM_WIDGET_H

//重写widget在item中显示
#include <QWidget>
#include<QPushButton>
#include<QLabel>
#include<QHBoxLayout>   //水平布局
#include<QEvent>

class item_widget : public QWidget
{
    Q_OBJECT
public:
    enum My_listwidget
    {
        listwidget,                 //音乐列表
        My_collection_list,         //我的收藏
        Recently_played_list,       //最近播放
        Answer_list                 //搜索的结果
    };

public:
    explicit item_widget(QWidget *parent = nullptr);
    void slotpressed();

    void setindex(int id, My_listwidget name, QString songname);        //获取widget指定的初值
    void setlove(bool lv);         //定义是否为收藏的
    int getindex();
    void enterEvent(QEvent *);      //鼠标进入
    void leaveEvent(QEvent *);      //鼠标移出

signals:
    void item_widget_index(int , My_listwidget, QString);   //暂停和开始
    void item_widget_love(int, My_listwidget,  QString, bool);                 //我的收藏

public:
    int id;
    My_listwidget list_name;
    QString songname;

    bool p_start;    //暂停和开始
    bool love;      //我的收藏

    QPushButton *like_it_Button;         //我喜欢的
    QPushButton *Pause_start_Button;     //暂停和开始
    QLabel *label;                       //歌曲名称

};

#endif // ITEM_WIDGET_H
