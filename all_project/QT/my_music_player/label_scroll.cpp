#include "label_scroll.h"

label_scroll::label_scroll(QWidget *parent) : QLabel(parent)
{
    timerID = -1;   //定时器的ID
    text_wpixel = 0;    //文本像素的长度

    speedt = 80;    //多久触发一次滚动
    spixel = 2;    //一次滚动多少像素

    flag = 0;   //默认不处理
}

label_scroll::~label_scroll()
{
    if(timerID >= 0)
        killTimer(timerID);
}

void label_scroll::setText(const QString &txt)
{
    //判断是否为富文本
    if(Qt::mightBeRichText(txt))
        flag = 0;
    else
        flag = 1;

    QLabel::setText(txt);
    upateLabelRollingState();
}

//判断是否启用滚动
void label_scroll::upateLabelRollingState()
{
    //获取文本所占的像素大小
    QFont ft = font();  //获取当前字体的格式，里面有文本的大小和文本像素大小
    QFontMetrics fm(ft);
    text_wpixel = fm.horizontalAdvance(text());
    //text_wpixel = rec.width();
    //qDebug() << "text_wpixel = " << text_wpixel;
    //qDebug() << "width = " << this->width();

    //长度超出本身label的像素大小，则开启滚动
    if(text_wpixel > this->width() && flag == 1)
    {
        left = 0;       //清空像素滚动量
        blank = " ";    //空格
        blank_wp = fm.horizontalAdvance(blank); //空格的像素宽度
        //qDebug() << "blank_wp = " << blank_wp;

        //开启定时器，定时器定时改变滚动量left
        timerID = startTimer(speedt);
    }
    else    //关闭文本滚动
    {
        flag = 0;   //关闭
        if(timerID >= 0)
        {
            killTimer(timerID); //关闭定时器
            timerID = -1;
        }
    }
}

//定时修改位移量，到末尾改为开头 负责修改当前像素位移值
void label_scroll::timerEvent(QTimerEvent *e)
{
    //isVisible判断控件是否可见
    if(e->timerId() == timerID && isVisible())
    {
        left += spixel;     //在左上角，每次增加对应的像素

        if(left > (text_wpixel + blank_wp))    //表示到达末尾
            left = 1-(this->width());        //重新添加，负数代表从右边开始

        update();
    }

    QLabel::timerEvent(e);
}

//重绘事件，根据位移量left显示文本
void label_scroll::paintEvent(QPaintEvent *e)
{
    if(flag == 0)
    {
        QLabel::paintEvent(e);
        return;
    }

    QPainter pen(this);
    //获取当前label的矩形大小
    QRect rc = rect();
    rc.setHeight(rc.height());
    rc.setWidth(rc.width());

    QString strText = this->text();     //在文本后面填充空格
    //QString strText = blank + this->text();     //在文本后面填充空格
    rc.setLeft(rc.left() - left);               //修改矩形，x轴，由于left在不断变大，setleft就在不断变小

    pen.drawText(rc,Qt::AlignVCenter,strText);  //根据给定的矩形坐标，绘制标签
}

//根据给定的数值，修改滚动的速度，sp是一次滚动多少像素，st是多少秒触发一次滚动
void label_scroll::setspeed(int sp, int st)
{
    spixel = sp;
    speedt = st;

    upateLabelRollingState();   //刷新一次滚动量
}

