#include "min_widow.h"
#include<QDebug>

Min_widow::Min_widow(QWidget *parent) : QWidget(parent)
{
    Initialization();

    connect(Close,&QPushButton::pressed,this,&Min_widow::slotClose);            //关闭
    connect(Reduction,&QPushButton::pressed,this,&Min_widow::slotReduction);    //还原
    connect(Last_song,&QPushButton::pressed,this,&Min_widow::slotLast_song);    //上一首
    connect(next_song,&QPushButton::pressed,this,&Min_widow::slotnext_song);    //下一首
    connect(start_stop,&QPushButton::pressed,this,&Min_widow::slotstart_stop);  //暂停和开始
}

Min_widow::~Min_widow()
{
    //delete Close;
    //delete Reduction;
}

//初始化
void Min_widow::Initialization()
{
    this->resize(650,150);
    //设置为无边框
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //设置控件与子控件背景透明化
    //this->setStyleSheet("background-color:rgba(0,0,0,0)");
    this->setAutoFillBackground(true);

    //关闭
    Close = new QPushButton(this);
    Close->setText("X");
    Close->resize(30,30);
    Close->move(this->width()-30,this->pos().ry());

    //返回主页面
    Reduction = new QPushButton(this);
    Reduction->setText("<<");
    Reduction->resize(30,30);
    Reduction->move(this->width()-60,this->pos().ry());

    //下一首
    next_song = new QPushButton(this);
    next_song->setText(">|");
    next_song->resize(100,100);
    next_song->move(this->width()-200,this->pos().ry()+25);

    //开始和暂停
    start_stop = new QPushButton(this);
    start_stop->setText("||");
    start_stop->resize(100,100);
    start_stop->move(this->width()-300,this->pos().ry()+25);

    //上一首
    Last_song = new QPushButton(this);
    Last_song->setText("|<");
    Last_song->resize(100,100);
    Last_song->move(this->width()-400,this->pos().ry()+25);

    //我的收藏
    love = new QPushButton(this);
    love->setText("收藏");
    love->resize(100,100);
    love->move(this->width()-500,this->pos().ry()+25);

    //歌名
    label = new label_scroll(this);
    label->resize(650,150);
    label->lower();

    //获取widget中所有的按钮并进行操作
    QList<QPushButton*> buttons = this->findChildren<QPushButton* >();

    foreach(auto bu, buttons)
    {
        bu->hide();
    }

    //只能用数字,用变量的话在函数中值就会变为乱码
}

void Min_widow::setname(QString name)
{
    label->setText(name);
}

//关闭窗口
void Min_widow::slotClose()
{
    this->close();
}

//隐藏窗口,并发送信号
void Min_widow::slotReduction()
{
    emit SingReduction();
    this->hide();

}

//上一首
void Min_widow::slotLast_song()
{
    emit singlast();
}

//下一首
void Min_widow::slotnext_song()
{
    emit singnext();
}

//暂停和开始
void Min_widow::slotstart_stop()
{
    emit singstart();
}

//窗口移动
void Min_widow::mousePressEvent(QMouseEvent *ev)
{
    if(ev->buttons() == Qt::LeftButton)
    {
        leftval = ev->globalPos() - this->pos();
    }
}

void Min_widow::mouseMoveEvent(QMouseEvent *ev)
{
    if(ev->buttons() == Qt::LeftButton)
    {
        this->move(ev->globalPos() - leftval);
    }
}

//鼠标进入
void Min_widow::enterEvent(QEvent *)
{
    QList<QPushButton*> buttons = this->findChildren<QPushButton* >();

    foreach(auto bu, buttons)
    {
        bu->show();
    }
}

//鼠标移出
void Min_widow::leaveEvent(QEvent *)
{
    QList<QPushButton*> buttons = this->findChildren<QPushButton* >();

    foreach(auto bu, buttons)
    {
        bu->hide();
    }
}
