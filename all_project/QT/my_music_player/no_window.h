#ifndef NO_WINDOW_H
#define NO_WINDOW_H

#include <QWidget>
#include<QMediaPlayer>
#include<QMediaPlaylist>
#include<QVector>
#include<QListWidgetItem>
#include<QSlider>
#include<QPalette>
#include<QLabel>

// 定义当前鼠标所处状态;
enum WindowStretchRectState
{
    NO_SELECT = 0,              // 鼠标未进入下方矩形区域;
    LEFT_TOP_RECT,              // 鼠标在左上角区域;
    TOP_BORDER,                 // 鼠标在上边框区域;
    RIGHT_TOP_RECT,             // 鼠标在右上角区域;
    RIGHT_BORDER,               // 鼠标在右边框区域;
    RIGHT_BOTTOM_RECT,          // 鼠标在右下角区域;
    BOTTOM_BORDER,              // 鼠标在下边框区域;
    LEFT_BOTTOM_RECT,           // 鼠标在左下角区域;
    LEFT_BORDER                 // 鼠标在左边框区域;
};

class No_window : public QWidget
{
    Q_OBJECT
public:
    explicit No_window(QWidget *parent = nullptr);
    ~No_window();

public slots:
    //计算拉伸区域Rect位置
    void CalculateCurrentStrechRect();
    //在窗口第一次show完之后需要计算拉伸区域rect位置
    void showEvent(QShowEvent *);
    //根据当前鼠标位置,定位鼠标具体哪一块拉伸区域
    WindowStretchRectState getCurrentStretchState(QPoint cursorPos);
    //根据getCurrentStretchState返回状态进行更新鼠标样式;
    void updateMouseStyle(WindowStretchRectState stretchState);
    //重写鼠标的移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //重写鼠标的点击事件
    void mousePressEvent(QMouseEvent *event);
    //拉伸窗口过程中,根据记录的坐标更新窗口的大小
    void updateWindowSize();

signals:

private:
    QRect m_leftTopRect;            //左上角
    QRect m_leftBottomRect;         //左下角
    QRect m_rightTopRect;           //右上角
    QRect m_rightBottomRect;        //右下角
    QRect m_topBorderRect;          //顶部边框
    QRect m_rightBorderRect;        //右边边框
    QRect m_bottomBorderRect;       //底部边框
    QRect m_leftBorderRect;         //左边边框

};

#endif // NO_WINDOW_H
