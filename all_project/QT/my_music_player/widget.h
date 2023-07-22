#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QMediaPlayer>
#include<QMediaPlaylist>
#include<QVector>
#include<QListWidgetItem>
#include<QSlider>
#include<QPalette>
#include<QLabel>
#include<QTimer>
#include<QPushButton>
#include<QListWidget>
#include<QHBoxLayout>   //水平布局
#include<QVBoxLayout>   //垂直布局
#include<QSpacerItem>
#include<QMetaEnum>
#include<QDialog>
#include<QTime>
#include<QMutex>
#include"myslider.h"
#include"min_widow.h"
#include"item_widget.h"


//区分正在使用哪一个列表
enum My_listwidget
{
    listwidget,                 //音乐列表
    My_collection_list,         //我的收藏
    Recently_played_list,       //最近播放
    Answer_list                 //搜索的结果
};

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

struct MediaObjectInfo{
    QString fileName;   //用于保存歌曲文件名
    QString filepath;   //用于保存歌曲文件路径
};

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void Initialization();                //初始化
    void mediaplayerInit();               //媒体播放器初始化
    void scanSongs();                     //扫描歌曲
    QVector<MediaObjectInfo> mediaObjectInfo;   //媒体信息存储
    void CalculateCurrentStrechRect();    //计算拉伸区域Rect位置
    void showEvent(QShowEvent *);         //在窗口第一次show完之后需要计算拉伸区域rect位置
    WindowStretchRectState getCurrentStretchState(QPoint cursorPos);        //根据当前鼠标位置,定位鼠标具体哪一块拉伸区域
    void updateMouseStyle(WindowStretchRectState stretchState);             //根据getCurrentStretchState返回状态进行更新鼠标样式;
    void listwidgethide();                //隐藏listwidget
    bool Recently_played_Duplicate_checking(int id);        //最近播放查重
    QString item_name();                     //获取item上面控件的歌名
    void add_list(My_listwidget listname, QString songname);      //加入我的收藏,最近播放
    void delete_list(My_listwidget listname, int id,int item_id);             //删除我的收藏,最近播放
    void shuffle();                             //随机播放,获取随机种子进行随机播放
    void unified_item_play();                   //统一所有列表的item的播放样式


    //重写事件
    void mouseMoveEvent(QMouseEvent *ev);      //重写鼠标的移动事件
    void mousePressEvent(QMouseEvent *ev);     //重写鼠标的点击事件
    void mouseReleaseEvent(QMouseEvent *);    //重写鼠标释放事件
    void updateWindowSize();                      //拉伸窗口过程中,根据记录的坐标更新窗口的大小
    void paintEvent(QPaintEvent *);               //绘制背景图
    void resizeEvent(QResizeEvent *);             //使得label随着主窗口的大小而变化
    void mouseDoubleClickEvent(QMouseEvent *);    //鼠标双击最大化/还原窗口

    //槽函数
    void slotonDuration(qint64 duration);     //获取播放总时长
    void slotonPosition(qint64 position);     //获取实时播放时间
    void slotSLtListWidgetClicked();            //歌曲列表,点击播放
    void slotval();                             //拖动滑块后的值
    void slotSliderPressed(qint64 val);         //歌曲快进到鼠标点击的位置
    void slotplayepressed(qint64 val);          //音量实时到鼠标点击位置
    void slotmediastatuschanged(QMediaPlayer::MediaStatus status);      //音乐自动更换后改变歌名
    void slotstateChanged(QMediaPlayer::State state);             //音乐播放状态;
    void slotpush_lider();                      //实时更新音量大小
    void slotTimerTimerOut();              //判断鼠标是否双击
    void slotReduction();                  //关闭小窗口,显示窗口
    void slotshowlistwidget();             //显示qlistwidget部件
    void slotRecently_played();         //最近播放
    void slotitem_start(int id, item_widget::My_listwidget name, QString songname);              //item_widget的点击播放暂停按钮
    void slotitem_love(int id, item_widget::My_listwidget name, QString songname, bool love);         //加入并同步我的收藏
    void slotcurrentIndex();                 //音乐列表索引发生改变的信号,统一所有列表的item图标


private slots:
    //暂停and开始
     void on_pushButton_clicked();
    //上一首
    void on_pushButton_2_clicked();
    //下一首
    void on_pushButton_3_clicked();
    //设置列表播放模式
    void on_pushButton_5_clicked();
    //搜索按钮
    void on_pushButton_4_clicked();
    //显示隐藏音量控制器
    void on_push_volume_clicked();
    //最小化窗口
    void on_min_clicked();
    //最大化窗口
    void on_max_clicked();
    //关闭窗口
    void on_close_clicked();

    void on_pushButton_6_clicked();

signals:
    void item_pressdsing();

private:
    Ui::Widget *ui;

    QRect m_leftTopRect;            //左上角
    QRect m_leftBottomRect;         //左下角
    QRect m_rightTopRect;           //右上角
    QRect m_rightBottomRect;        //右下角
    QRect m_topBorderRect;          //顶部边框
    QRect m_rightBorderRect;        //右边边框
    QRect m_bottomBorderRect;       //底部边框
    QRect m_leftBorderRect;         //左边边框

    QRect   window_Beginning;               //窗体改变前的位置及大小
    WindowStretchRectState stretchState;    //鼠标样式
    QPoint m_startPoint;            //拉伸起点
    QPoint m_endPoint;              //拉伸终点
    Min_widow m_window;             //小窗口

    QTimer m_cTimer;                //定时器用来区分单双击鼠标

    QPalette m_pal;        //调色板
    Myslider *lider;          //音量控制器
    bool lider_bool;         //控制音量控制器的显示和隐藏

    QLabel *imglabel;       //动态背景
    QPoint Diff_val;        //窗口位置与鼠标位置的差值


    QListWidget *My_collection;      //我的收藏
    QListWidget *Recently_played;   //最近播放
    QListWidget *Answer;            //搜索得到的答案

    //存储所有的歌曲名
    QVector<QString> total_song_name_Array; //存储所有的歌曲名
    int total_song_val_array[300];      //存错所有的歌曲名在音乐列表中的播放位置

    //列表容器
    //下标表示行号,值表示歌曲序列号,帮助播放器定位每个列表要播放歌曲的位置
    QVector<int> listwidget_Array;
    QVector<int> My_collection_Array;
    QVector<int> Recently_played_Array;
    QVector<int> Answer_Array;

    //歌曲数组
    //下标对应的歌曲序列号,值表示所在列表的行号,用来统一修改收藏符号
    int listwidget_array[300];
    int my_collection_array[300];
    int recently_player_array[300];
    int answer_array[300];

    //保存随机播放的位置,上一曲的时候使用
    QVector<int> listwidget_random;
    QVector<int> my_collection_random;
    QVector<int> recently_played_random;
    QVector<int> answer_random;

    //item的信号值
    QVector<int> listwidget_ID;
    QVector<int> my_collection_ID;
    QVector<int> recently_played_ID;

    //列表的序号值
    QVector<int> listwidget_id;
    QVector<int> my_collection_id;
    QVector<int> recently_played_id;

    //ID复用
    QVector<int> listwidget_deleteID;
    QVector<int> my_collection_deleteID;
    QVector<int> recently_played_deleteID;

    int playlist_val;

    QMutex sing_tmuex;

    //我的收藏假删除,如果在我的收藏列表取消收藏是不会删除的,只有在切换列表后才会进行删除
    QVector<int> my_collection_delete_false;

    My_listwidget list;             //区分正在使用哪一个列表

    QMediaPlayer *player;   //播放器
    QMediaPlaylist *playlist;                   //音乐播放列表

    QString durationTime;   //总长度
    QString positionTime;   //当前播放位置
    QList<QString>  lyricsTextList; //歌词文本列表
    QList<qint64>   lyricsTimeList;//歌词时间列表
};
#endif // WIDGET_H
