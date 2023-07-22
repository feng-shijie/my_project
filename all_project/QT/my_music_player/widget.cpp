#include "widget.h"
#include "ui_widget.h"
#include<QDir>
#include<QDebug>
#include<QMovie>
#include<QImage>
#include<QStringList>

#define STRETCH_RECT_HEIGHT 4       // 拉伸小矩形的高度;
#define STRETCH_RECT_WIDTH 4        // 拉伸小矩形的宽度;


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //初始化窗口大小
    this->resize(1800,1024);

    Initialization();
    scanSongs();

    //媒体信号槽链接
    connect(player,&QMediaPlayer::durationChanged,this,&Widget::slotonDuration);                //获取音乐总时长
    connect(player,&QMediaPlayer::positionChanged,this,&Widget::slotonPosition);                //实时更新音乐时间
    connect(player,&QMediaPlayer::mediaStatusChanged,this,&Widget::slotmediastatuschanged);     //音乐播放结束,自动播放下一首,加入最近播放

    //playlist音乐列表信号
    //connect(playlist,&QMediaPlaylist::currentIndexChanged,this,&Widget::slotcurrentIndex);  //音乐列表索引发生改变,统一所有的item图标

    //listwidget槽链接
    connect(ui->listWidget,&QListWidget::itemDoubleClicked,this,&Widget::slotSLtListWidgetClicked);     //双击列表播放该歌曲
    connect(My_collection,&QListWidget::itemDoubleClicked,this,&Widget::slotSLtListWidgetClicked);
    connect(Recently_played,&QListWidget::itemDoubleClicked,this,&Widget::slotSLtListWidgetClicked);
    connect(Answer,&QListWidget::itemDoubleClicked,this,&Widget::slotSLtListWidgetClicked);
    connect(ui->Function,&QListWidget::itemPressed,this,&Widget::slotshowlistwidget);           //点击function列表切换listwidget页面

    //Qslider槽链接
    connect(ui->horizontalSlider,&QSlider::sliderReleased,this,&Widget::slotval);           //拖动进度条改变音乐时间
    connect(ui->horizontalSlider,&Myslider::playersign,this,&Widget::slotSliderPressed);    //点击进度条改变音乐时间

    //Qslider槽链接
    connect(lider,&QSlider::valueChanged,this,&Widget::slotpush_lider);         //拖动进度条改变音乐时间
    connect(lider,&Myslider::playersign,this,&Widget::slotplayepressed);        //点击进度条改变音乐时间

    //计时器槽链接
    connect(&m_cTimer,&QTimer::timeout,this,&Widget::slotTimerTimerOut);        //双击窗口放大缩小定时器

    //min_window槽链接
    connect(&m_window,&Min_widow::SingReduction,this,&Widget::slotReduction);           //小窗口关闭,窗口还原
    connect(&m_window,&Min_widow::singlast,this,&Widget::on_pushButton_2_clicked);      //上一首
    connect(&m_window,&Min_widow::singnext,this,&Widget::on_pushButton_3_clicked);      //下一首
    connect(&m_window,&Min_widow::singstart,this,&Widget::on_pushButton_clicked);       //暂停开始

}

Widget::~Widget()
{
    delete ui;
}

//初始化
void Widget::Initialization()
{   
    //初始化列表媒体播放器
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    playlist->clear();                                  //清空播放器列表
    player->setPlaylist(playlist);                      //设置播放器的列表为playlist
    playlist->setPlaybackMode(QMediaPlaylist::Loop);    //设置播放模式，loop是列表循环

    //初始化所有数组
    memset(listwidget_array,0,sizeof(listwidget_array));
    memset(my_collection_array,0,sizeof(my_collection_array));
    memset(recently_player_array,0,sizeof(recently_player_array));
    memset(answer_array,0,sizeof(answer_array));
    memset(total_song_val_array,0,sizeof(total_song_val_array));

    playlist_val = 0;
    list = My_listwidget::listwidget;       //初始化为音乐列表


    //固定label_3的大小
    ui->label_3->setMaximumWidth(100);
    ui->label_3->setMinimumWidth(100);

    //定义字体
    QFont fn("宋体",10,87);
    ui->label_3->setFont(fn);
    ui->Function->setFont(fn);
    ui->label->setFont(fn);
    ui->label_2->setFont(fn);
    ui->min->setFont(fn);
    ui->max->setFont(fn);
    ui->close->setFont(fn);
    ui->pushButton_4->setFont(fn);
    ui->pushButton_6->setFont(fn);

    //提前触发关于滚动所需要的条件
    ui->label_3->setText("网易云音乐");
    m_window.setname(ui->label_3->text());  //传递歌曲名

    //指定父对象
    lider = new Myslider(this);

    lider->hide();
    lider_bool = false;
    //设置音量与播放器同步
    lider->setValue(player->volume());

    //设置音量的最大值和最小值
    lider->setMaximum(100);
    lider->setMinimum(0);
    //设置音量显示大小
    lider->resize(30,121);
    //移动部件至指定位置
    lider->move(ui->push_volume->pos().rx(),ui->push_volume->pos().ry() - 121);


    //设置widget无边框
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

#if 0
    //设置widget的背景图片 用label更好用一点
    this->setAutoFillBackground(true);
    m_pal = this->palette();
    m_pal.setBrush(QPalette::Background,QBrush(QPixmap("C:/Users/26035/Desktop/image/dm.jpg")));
    setPalette(m_pal);
#endif

    //设置控件与子控件的背景透明化
    this->setStyleSheet("background-color:rgba(0,0,0,0)");
#if 0
    //设置控件背景透明化，不设置子控件的
    QPalette pll = this->palette();
    pll.setBrush(QPalette::Base,QBrush(QColor(255,255,255,0)));
    this->setPalette(pll);
#endif

    imglabel = new QLabel(this);
# if 0
    //设置动态壁纸
    QMovie *movie = new QMovie("C:/Users/26035/Desktop/image/dt_3.gif");

    //imglabel -> setGeometry(0,0,this->width(),this->height());
    //将动图插入标签
    imglabel -> setMovie(movie);
    //播放动态图片，否则图片无法显示
    movie->start();
#endif
    //修改label的大小
    imglabel->resize(this->width(),this->height());
    //使得图像自适应label大小
    imglabel->setScaledContents(true);
    //设置label显示在控件最下方
    imglabel->lower();

    //设置静态壁纸
    QImage *img = new QImage;
    img->load("C:/Users/26035/Desktop/image/fj.jpg");
    imglabel->setPixmap(QPixmap::fromImage(*img));
    //实时追踪鼠标位置
    this->setMouseTracking(true);
    //因为动态背景使label在显示的所以需要开启此部件的鼠标追踪
    imglabel->setMouseTracking(true);


    //ui->pushButton_2->setIcon(QIcon("C:/Users/26035/Desktop/image/Last_song.png"));
    //ui->pushButton_3->setIcon(QIcon("C:/Users/26035/Desktop/image/nextt.png"));
    ui->pushButton_2->setStyleSheet("border-image: url(C:/Users/26035/Desktop/image/Last_song.png)");
    ui->pushButton_3->setStyleSheet("border-image: url(C:/Users/26035/Desktop/image/nextt.png)");
    //ui->pushButton->setFlat(true);
    ui->pushButton->setText("暂停");

    ui->Function->setMaximumWidth(200);
    ui->Function->addItems(QStringList(QStringList() << "音乐列表" << "我的收藏" << "最近播放" << "每日推荐"));
    //ui->Function->setCurrentRow(1);

    My_collection = new QListWidget(this);
    Recently_played = new QListWidget(this);
    Answer = new QListWidget(this);

    listwidgethide();       //隐藏所有listwidget                                  ^
    ui->listWidget->show(); //显示一下ui的listwidget

    //设置进度条的样式表
    ui->horizontalSlider->setStyleSheet("QSlider::groove:horizontal {border: 0px solid #bbb;}"
                                        "QSlider::add-page:horizontal{background-color:rgb(255,255,255);width:4px;border:0px solid #777;border-radius:2px;margin-top:9px;margin-bottom:9px;}"
                                        "QSlider::sub-page:horizontal{background-color:rgb(90,49,255);width:4px;border-radius:2px;margin-top:8px;margin-bottom:8px;}"
                                        "QSlider::handle:horizontal{background:rgb(255,255,255);border:1px solid rgba(102,102,102,102);width:8px;height:8px;border-radius:5px;margin-top:2px;margin-bottom:2px;}"
                                        "QSlider::handle:horizontal:hover{background:rgb(255,255,0);border:1px solid rgba(102,102,102,102);border-radius:5px}"
                                        "QSlider::sub-page:horizontal{background:#bbb;border-color:#999;}");
    //设置音量条的样式表
    lider->setStyleSheet("QSlider{background:transparent;}"
                         "QSlider::add-page:vertical{background-color:rgb(90,49,255);width:4px;border:0px solid #777;border-radius:2px;margin-top:9px;margin-bottom:9px;}"
                         "QSlider::sub-page:vertical{background-color:rgba(196,196,196,0.5);width:4px;border-radius:2px;margin-top:8px;margin-bottom:8px;}"
                         "QSlider::groove:vertical{background:transparent;width:8px;border-radius:2px;}"
                         "QSlider::handle:vertical{background:rgb(193,204,208);border:1px solid rgb(193,204,208);width:8px;height:8px;border-radius:2px;margin-top:2px;margin-bottom:2px;}"
                         "QSlider::handle:vertical:hover{background:rgb(193,204,208);border:1px solid rgb(193,204,208);border-radius:5px;}");

    //ui->pushButton->setStyleSheet("QPushButton{border-image: url(C:/Users/HP/Desktop/image/321.jpg);border-width:0px");
}

//扫描歌曲
void Widget::scanSongs()
{
    QDir dir("D:\\music");
    QDir dir1(dir.absolutePath());

    if(dir1.exists())
    {
        QStringList filter;
        filter << "*.mp3";
        QFileInfoList files = dir1.entryInfoList(filter,QDir::Files);

        for(int i=0; i<files.count(); ++i)
        {
            MediaObjectInfo info;
            QString fileName = QString::fromUtf8(files.at(i).fileName().replace(".mp3","").toUtf8().data());
            info.fileName = fileName;
            info.filepath = QString::fromUtf8(files.at(i).filePath().toUtf8().data());

            //媒体列表添加歌曲,把歌曲添加到总列表
            if(playlist->addMedia((QUrl::fromLocalFile(info.filepath))))
            {
                //listwidget_array[i] = i;    //我的收藏与列表歌曲相对应
                //添加到容器数组存储
                mediaObjectInfo.append(info);
                total_song_name_Array.push_back(info.fileName);  //歌曲名添加到总的歌曲名容器
                total_song_val_array[i] = i;               //保存歌曲名在音乐列表中的播放位置
                listwidget_Array.push_back(i);              //列表歌名与音乐列表播放位置相对应
                listwidget_array[i] = i;                    //歌曲序列号对应列表行号
                listwidget_ID.push_back(ui->listWidget->count());      //下标是当前列表序列号,值是item的id值
                listwidget_id.push_back(ui->listWidget->count());      //下标是item的id值,值是所属的列表序列号

                //初始化列表框中小列表项
                QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
                item->setSizeHint(QSize(100,60));
                 item_widget *w = new item_widget(ui->listWidget);

                 w->setindex(ui->listWidget->count()-1,item_widget::My_listwidget::listwidget,info.fileName);
                ui->listWidget->setItemWidget(item,w);

                connect(w,&item_widget::item_widget_index,this,&Widget::slotitem_start);
                connect(w,&item_widget::item_widget_love,this,&Widget::slotitem_love);

            }
            else
            {
                qDebug() << "Error number:" << playlist->error() << endl;
            }
        }
    }
    else
    {
        //路劲不存在
        qDebug() << "path not exist!" << endl;
    }
}

//获取播放总时间
void Widget::slotonDuration(qint64 duration)
{
    //获取文件总时长
    duration = player->duration();

    ui->horizontalSlider->setValue(0);
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(duration);

    int h, m;

    duration /= 1000;
    h = duration/60;
    m = duration % 60;

    if(m < 10)
        durationTime = QString::asprintf("0%d:0%d",h,m);
    else
        durationTime = QString::asprintf("0%d:%d",h,m);
    //durationTime = QString::asprintf("%d:%d",h,m);
    ui->label->setText(positionTime+ "/" +durationTime);
}

//音乐播放完毕判断当前播放模式自动播放下一首,正在播放的歌曲全部加入最近播放
void Widget::slotmediastatuschanged(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::EndOfMedia)  //自动播放下一首
    {
        unified_item_play();    //统一item的播放图标

        //如果是单曲循环直接退出
        if(QMediaPlaylist::CurrentItemInLoop == playlist->playbackMode())   return;

        //playlist->playbackMode()::获取当前的播放模式
        else if(QMediaPlaylist::Loop == playlist->playbackMode())    //循环播放
        {
            if(list == My_listwidget::listwidget)
            {
                //如果大于等于总值就重新开始播放
                if(ui->listWidget->currentRow()+1 > ui->listWidget->count()-1)
                {
                    ui->listWidget->setCurrentRow(0);
                    playlist->setCurrentIndex(listwidget_Array[0]);
                    return;
                }

                ui->listWidget->setCurrentRow(ui->listWidget->currentRow()+1);
                playlist->setCurrentIndex(listwidget_Array[ui->listWidget->currentRow()]);
            }
            else if(list == My_listwidget::My_collection_list)
            {
                if(My_collection->currentRow()+1 > My_collection->count())
                {
                    My_collection->setCurrentRow(0);
                    playlist->setCurrentIndex(My_collection_Array[0]); return;
                }

                My_collection->setCurrentRow(My_collection->currentRow()+1);
                playlist->setCurrentIndex(My_collection_Array[My_collection->currentRow()]);
            }
            else if(list == My_listwidget::Recently_played_list)
            {
                if(Recently_played->currentRow()+1 > Recently_played->count()-1)
                {
                    Recently_played->setCurrentRow(0);
                    playlist->setCurrentIndex(Recently_played_Array[0]); return;
                }

                Recently_played->setCurrentRow(Recently_played->currentRow()+1);
                playlist->setCurrentIndex(Recently_played_Array[Recently_played->currentRow()]);
            }
            else if(list == My_listwidget::Answer_list)
            {
                if(Answer->currentRow()+1 > Answer->count()-1)
                {
                    Answer->setCurrentRow(0);
                    playlist->setCurrentIndex(Answer_Array[0]); return;
                }

                Answer->setCurrentRow(Answer->currentRow()+1);
                playlist->setCurrentIndex(Answer_Array[Answer->currentRow()]);
            }
        }
        else if(QMediaPlaylist::Random == playlist->playbackMode())    //随机播放
        {
            shuffle();  //处理随机播放
        }

        item_name();
    }

    //当前正在播放的歌曲加入最进播放
    if(status == QMediaPlayer::BufferedMedia)
    {
        int val = playlist->currentIndex();     //获取当前歌曲序列号
        if(list == My_listwidget::Recently_played_list)
        {
            unified_item_play();
            return;
        }        
        else if(list == My_listwidget::listwidget)
        {
            //只保存9首歌曲,如果有重复的直接退出,传入当前歌曲序列号
            if(Recently_played_Duplicate_checking(val))
            {
                unified_item_play();    return;
            }
           //把当前播放的歌曲序号加入到最近播放列表所新增的行数中
           Recently_played_Array.insert(Recently_played_Array.begin(),val);
           //把当前的位置添加到歌曲数组
           recently_player_array[val] = Recently_played->count();
        }
        else if(list == My_listwidget::My_collection_list)
        {
            if(Recently_played_Duplicate_checking(val))
            {
                unified_item_play();    return;
            }
            Recently_played_Array.insert(Recently_played_Array.begin(),val);
            recently_player_array[val] = Recently_played->count();
        }
        else if(list == My_listwidget::Answer_list)
        {
            if(Recently_played_Duplicate_checking(val))
            {
                unified_item_play();    return;
            }
        Recently_played_Array.insert(Recently_played_Array.begin(),val);
        recently_player_array[val] = Recently_played->count();
        }

        add_list(Recently_played_list,ui->label_3->text()); //加入最近播放
        unified_item_play();        //统一item播放标志
    }
}

//当音乐列表发生改变就修改所有列表中上一首item的图标
void Widget::slotcurrentIndex()
{
    int tem = playlist_val;
    //统一item上面的暂停播放图标
    if(listwidget_Array[listwidget_array[tem]] == tem)
    {
        ui->listWidget->setCurrentRow(listwidget_array[tem]);
        QWidget *w = ui->listWidget->itemWidget(ui->listWidget->currentItem());
        QPushButton *bu = w->findChild<QPushButton *>("Pause_start_Button");
        //统一播放样式
        bu->setText("暂停");
    }
    if(!Recently_played_Array.isEmpty())
    {
        if(Recently_played_Array[Recently_played->count()-1 - recently_player_array[tem]] == tem)
        {
            Recently_played->setCurrentRow(Recently_played->count()-1 - recently_player_array[tem]);
            QWidget *w = Recently_played->itemWidget(Recently_played->currentItem());
            QPushButton *bu = w->findChild<QPushButton *>("Pause_start_Button");
            bu->setText("暂停");
        }
    }
    if(!My_collection_Array.isEmpty())
    {
       if(My_collection_Array[My_collection->count()-1 - my_collection_array[tem]] == tem)
       {
           My_collection->setCurrentRow(My_collection->count()-1 - my_collection_array[tem]);
           QWidget *w = My_collection->itemWidget(My_collection->currentItem());
           QPushButton *bu = w->findChild<QPushButton *>("Pause_start_Button");
           bu->setText("暂停");
       }
    }
    if(!Answer_Array.isEmpty())
    {
        if(Answer_Array[answer_array[tem]] == tem)
        {
            Answer->setCurrentRow(answer_array[tem]);
            QWidget *w = Answer->itemWidget(Answer->currentItem());
            QPushButton *bu = w->findChild<QPushButton *>("Pause_start_Button");
            bu->setText("暂停");
        }
    }
}

//实时更新歌曲播放时间
void Widget::slotonPosition(qint64 position)
{
    //获取歌曲的当前位置
    position = player->position();
    //修改进度条的位置
    ui->horizontalSlider->setSliderPosition(position);

    int h, m;
    position /= 1000;
    h = position/60;
    m = position % 60;

    if(m < 10)
        positionTime = QString::asprintf("0%d:0%d",h,m);
    else
        positionTime = QString::asprintf("0%d:%d",h,m);

    ui->label->setText(positionTime+ "/" +durationTime);
}

//进度条>>释放滑块及发送信号
void Widget::slotval()
{
    qint64 val;
    //获取滑块修改后的当前值
    val = ui->horizontalSlider->value();
    //把修改后的值给到播放控件
    player->setPosition(val);
}

//歌曲快进到鼠标点击的位置
void Widget::slotSliderPressed(qint64 val)
{
    player->setPosition(val);
}

//歌曲列表双击击，播放此项
void Widget::slotSLtListWidgetClicked()
{
    QObject *sender = QObject::sender();

    player->stop();

    //歌曲列表
    if(sender == ui->listWidget)
    {
        if(list != My_listwidget::listwidget)
        {
            list = My_listwidget::listwidget;       //申明正在使用在这个列表
            //player->setPlaylist(playlist);
        }
        //改变音乐列表中当前开始播放的起始位置
        //playlist->setCurrentIndex(ui->listWidget->currentRow());
        //直接使用列表容器中的值,即歌曲序列号
        playlist->setCurrentIndex(listwidget_Array[ui->listWidget->currentRow()]);

        //输出当前对象的位置
        //qDebug() << ui->listWidget->currentRow() << endl;
        //获取所选行的名称
        //qDebug() <<  ui->listWidget->currentItem()->text() << endl;
    }

    //我的收藏
    if(sender == this->My_collection)
    {
        if(list != My_listwidget::My_collection_list)
        {
            list = My_listwidget::My_collection_list;       //申明正在使用在这个列表
            //player->setPlaylist(My_collection_playlist);
        }

        int tem = My_collection_Array[My_collection->currentRow()];
        playlist->setCurrentIndex(tem);
    }

    //最近播放
    if(sender == this->Recently_played)
    {
        if(list != My_listwidget::Recently_played_list)
        {
            list = My_listwidget::Recently_played_list;       //申明正在使用在这个列表
            //player->setPlaylist(Recently_playlist);
        }

        playlist->setCurrentIndex(Recently_played_Array[Recently_played->currentRow()]);

    }

    //搜索得到的答案
    if(sender == this->Answer)
    {
        if(list != My_listwidget::Answer_list)
        {
            list = My_listwidget::Answer_list;       //申明正在使用在这个列表
            //player->setPlaylist(Answer_playlist);
        }
        playlist->setCurrentIndex(Answer_Array[Answer->currentRow()]);
    }

    item_name();
    ui->pushButton->setText("开始");
    player->play();
}

//实时更新音量大小
void Widget::slotpush_lider()
{
    player->setVolume(lider->value());
}

//音量实时到鼠标点击位置
void Widget::slotplayepressed(qint64 val)
{
    lider->setValue(val);
    player->setVolume(lider->value());
}

//更换listwidget界面
void Widget::slotshowlistwidget()
{
    QObject *sender = QObject::sender();

    if(sender == ui->Function)
    {
        int row = ui->Function->currentRow();

        switch(row)
        {
        case 0:
            listwidgethide();
            ui->listWidget->show();
            break;
        case 1:
            listwidgethide();
            My_collection->resize(ui->listWidget->size());
            My_collection->move(ui->listWidget->pos());
            My_collection->show();
            break;
        case 2:
            listwidgethide();
            Recently_played->resize(ui->listWidget->size());
            Recently_played->move(ui->listWidget->pos());
            Recently_played->show();
            break;
        default:
            break;
        }
    }

    //不行还是要一直判断
    if(sender == ui->listWidget)
    {
        list = My_listwidget::listwidget;
        emit item_pressdsing();         //向子进程item发射信
    }
    if(sender == My_collection)
    {
        list = My_listwidget::My_collection_list;
        emit item_pressdsing();         //向子进程item发射信
    }
    if(sender == Recently_played)
    {
        list = My_listwidget::Recently_played_list;
        emit item_pressdsing();         //向子进程item发射信
    }
    if(sender == Answer)
    {
        list = My_listwidget::Answer_list;
        emit item_pressdsing();         //向子进程item发射信
    }

}

//item_widget的点击播放暂停按钮
void Widget::slotitem_start(int id, item_widget::My_listwidget name, QString songname)
{
    int tem;
    //如果当前播放数与音乐播放列表数不同则修改播放列表然后播放,如果相同则调用别的函数后退出
    if(name == item_widget::My_listwidget::listwidget)
    {
        if(list != My_listwidget::listwidget)
        {
            list = My_listwidget::listwidget;
        }
        if(ui->listWidget->currentRow() == listwidget_id[id])
        {
            on_pushButton_clicked();
            return;
        }
        ui->listWidget->setCurrentRow(listwidget_id[id]);
        playlist->setCurrentIndex(listwidget_Array[listwidget_id[id]]);
    }

    if(name == item_widget::My_listwidget::My_collection_list)
    {
        if(list != My_listwidget::My_collection_list)
        {
            list = My_listwidget::My_collection_list;
        }
        tem = My_collection->count()-1 -my_collection_id[id];
        if(My_collection->currentRow() == tem)
        {
            on_pushButton_clicked();
            return;
        }
        My_collection->setCurrentRow(tem);
        playlist->setCurrentIndex(My_collection_Array[tem]);
    }

    if(name == item_widget::My_listwidget::Recently_played_list)
    {
        if(list != My_listwidget::Recently_played_list)
        {
            list = My_listwidget::Recently_played_list;
        }
        tem = Recently_played->count()-1 - recently_played_id[id];
        if(Recently_played->currentRow() == tem)
        {
            on_pushButton_clicked();
            return;
        }

        Recently_played->setCurrentRow(tem);
        playlist->setCurrentIndex(Recently_played_Array[tem]);
    }

    if(name == item_widget::My_listwidget::Answer_list)
    {
        if(list != My_listwidget::Answer_list)
        {
            list = My_listwidget::Answer_list;
        }
        if(Answer->currentRow() == id)
        {
            on_pushButton_clicked();
            return;
        }
        Answer->setCurrentRow(id);
        playlist->setCurrentIndex(Answer_Array[id]);
    }

    ui->label_3->setText(songname);
    m_window.setname(songname);     //传递歌曲名称
    player->play();
}

//加入我的收藏,并同步所有列表
void Widget::slotitem_love(int id, item_widget::My_listwidget name, QString songname, bool love)
{
    int tem,row;
    if(love == true)    //加入收藏
    {
        //收藏列表中只能撤销收藏
        if(name == item_widget::My_listwidget::My_collection_list)  return;

        else if(name == item_widget::My_listwidget::listwidget)  //音乐列表统一
        {
            if(id == listwidget_array[listwidget_Array[listwidget_id[id]]])  //音乐列表
            {
                My_collection_Array.insert(My_collection_Array.begin(),listwidget_Array[listwidget_id[id]]);    //每次加入到第一行
                my_collection_array[listwidget_Array[listwidget_id[id]]] = My_collection->count();
                add_list(My_collection_list,songname);
            }

            if(!Recently_played_Array.isEmpty())
            {
                //tem是最近播放中的列表行,因为最近播放是使用的栈结构的
                row = Recently_played->count()-1 - recently_player_array[listwidget_Array[listwidget_id[id]]];
                if(Recently_played_Array[row] == listwidget_Array[listwidget_id[id]]) //最近播放
                {
                    Recently_played->setCurrentRow(row);    //移动到指定行进行操作
                    //在item上面获取自定义控件,其中按钮这个控件需要在自定义声明文件中使用setObjectName进行命名
                    //需要在按钮定义的地方使用setObjectName给对象命名这样才能通过findChild查找到指定对象
                    QWidget *w = Recently_played->itemWidget(Recently_played->currentItem());
                    QPushButton *love = w->findChild<QPushButton *>("like_it_Button");
                    love->setText("❤");
                }
            }
        }

        else if(name == item_widget::My_listwidget::Recently_played_list)    //最近播放统一
        {
            row = Recently_played->count()-1 - recently_played_id[id];      //获取当前所在行
            if(row == Recently_played->count()-1 - recently_player_array[Recently_played_Array[row]])  //最近播放
            {
                My_collection_Array.insert(My_collection_Array.begin(),Recently_played_Array[row]);
                my_collection_array[Recently_played_Array[row]] = My_collection->count();
                add_list(My_collection_list,songname);
            }

            if(listwidget_Array[listwidget_array[Recently_played_Array[row]]] == Recently_played_Array[row]) //音乐列表
            {
                tem = ui->listWidget->currentRow();
                ui->listWidget->setCurrentRow(listwidget_array[Recently_played_Array[row]]);  //移动到指定行进行操作
                QWidget *w = ui->listWidget->itemWidget(ui->listWidget->currentItem());
                QPushButton *love = w->findChild<QPushButton *>("like_it_Button");
                love->setText("❤");
                ui->listWidget->setCurrentRow(tem);
            }
        }

        else if(name == item_widget::My_listwidget::Answer_list)     //搜索列表统一
        {
            if(id == answer_array[Answer_Array[id]])  //最近播放
            {
                My_collection_Array.insert(My_collection_Array.begin(),Answer_Array[id]);
                my_collection_array[Answer_Array[id]] = My_collection->count();
                add_list(My_collection_list,songname);
            }

            if(listwidget_Array[listwidget_array[Answer_Array[id]]] == Answer_Array[id]) //音乐列表
            {
                tem = ui->listWidget->currentRow();
                ui->listWidget->setCurrentRow(listwidget_array[Answer_Array[id]]);  //移动到指定行进行操作
                QWidget *w = ui->listWidget->itemWidget(ui->listWidget->currentItem());
                QPushButton *love = w->findChild<QPushButton *>("like_it_Button");
                love->setText("❤");
                ui->listWidget->setCurrentRow(tem);
            }

            if(!Recently_played_Array.isEmpty())
            {
                if(Recently_played_Array[Recently_played->count()-1 - recently_player_array[Answer_Array[id]]] == Answer_Array[id]) //最近播放
                {
                    tem = Recently_played->currentRow();
                    Recently_played->setCurrentRow(Recently_played->count()-1 - recently_player_array[Answer_Array[id]]);    //移动到指定行进行操作
                    QWidget *w = Recently_played->itemWidget(Recently_played->currentItem());
                    QPushButton *love = w->findChild<QPushButton *>("like_it_Button");
                    love->setText("❤");
                    Recently_played->setCurrentRow(tem);
                }
            }
        }

    }

    //移除收藏的id值是当前歌曲的序列号
    else if(love == false)   //移除我的收藏
    {
        if(name == item_widget::My_listwidget::listwidget)  //音乐列表
        {
            if(listwidget_array[listwidget_id[id]] == listwidget_array[listwidget_Array[listwidget_id[id]]])    //音乐列表
            {
                delete_list(My_collection_list,listwidget_Array[listwidget_id[id]],id);      //删除我的收藏
            }
            if(!Recently_played_Array.isEmpty())
            {
               if(Recently_played_Array[Recently_played->count()-1 - recently_player_array[listwidget_Array[listwidget_id[id]]]]
                       == recently_player_array[listwidget_Array[listwidget_id[id]]]) //最近播放
               {
                   tem = Recently_played->currentRow();
                   Recently_played->setCurrentRow(Recently_played->count()-1 - recently_player_array[listwidget_Array[listwidget_id[id]]]);
                   QWidget *w = Recently_played->itemWidget(Recently_played->currentItem());
                   QPushButton *love = w->findChild<QPushButton *>("like_it_Button");
                   love->setText("收藏");
                   Recently_played->setCurrentRow(tem);
               }
            }
        }

        else if(name == item_widget::My_listwidget::Recently_played_list)   //最近播放
        {
            row = Recently_played->count()-1 - recently_played_id[id];
            if(row == Recently_played->count()-1 - recently_player_array[Recently_played_Array[row]]) //最近播放
            {
                delete_list(My_collection_list,Recently_played_Array[row],id);      //删除我的收藏
            }
            if(Recently_played_Array[row] == listwidget_Array[listwidget_array[Recently_played_Array[row]]])  //音乐列表
            {
                tem = ui->listWidget->currentRow();
                ui->listWidget->setCurrentRow(listwidget_array[Recently_played_Array[row]]);
                QWidget *w = ui->listWidget->itemWidget(ui->listWidget->currentItem());
                QPushButton *love = w->findChild<QPushButton *>("like_it_Button");
                love->setText("收藏");
                ui->listWidget->setCurrentRow(tem);
            }
        }

        else if(name == item_widget::My_listwidget::My_collection_list) //我的收藏
        {
            row = My_collection->count()-1 -my_collection_id[id];   //目前所在行
            if( My_collection_Array[row] == listwidget_Array[listwidget_array[My_collection_Array[row]]])  //音乐列表
            {
                tem = ui->listWidget->currentRow();
                ui->listWidget->setCurrentRow(listwidget_array[My_collection_Array[row]]);
                QWidget *w = ui->listWidget->itemWidget(ui->listWidget->currentItem());
                QPushButton *love = w->findChild<QPushButton *>("like_it_Button");
                love->setText("收藏");
                ui->listWidget->setCurrentRow(tem);
            }
            if(!Recently_played_Array.isEmpty())
            {
              if(My_collection_Array[row] == Recently_played_Array[Recently_played->count()-1 - recently_player_array[My_collection_Array[row]]]) //最近播放
              {
                  tem = Recently_played->currentRow();
                  Recently_played->setCurrentRow(Recently_played->count()-1 - recently_player_array[My_collection_Array[row]]);
                  QWidget *w = Recently_played->itemWidget(Recently_played->currentItem());
                  QPushButton *love = w->findChild<QPushButton *>("like_it_Button");
                  love->setText("收藏");
                  Recently_played->setCurrentRow(tem);
              }
            }
            if(row == My_collection->count()-1 - my_collection_array[My_collection_Array[row]])  //取消收藏以后先不删除,等切换页面后在统一删除
            {
                delete_list(My_collection_list, My_collection_Array[row],id);      //删除我的收藏
            }
        }
    }

}

//加入我的收藏,最近播放
void Widget::add_list(My_listwidget listname, QString songname)
{
    //初始化列表框中小列表项
     QListWidgetItem *item = new QListWidgetItem();
     item->setSizeHint(QSize(100,60));

    if(listname == My_listwidget::My_collection_list)   //我的收藏
    {
        My_collection->insertItem(0,item);      //每次添加到第一行
        item_widget *w = new item_widget(My_collection);

        if(!my_collection_deleteID.isEmpty())       //id复用
        {
            my_collection_ID.insert(my_collection_ID.begin(),my_collection_deleteID[0]);      //优先使用复用id
            my_collection_id[my_collection_deleteID[0]] = My_collection->count()-1;            //id值等于列表的总数减一刚插入进来的
            w->setindex(my_collection_deleteID[0],item_widget::My_listwidget::My_collection_list,songname);
            qDebug() << "id:" << my_collection_id[my_collection_deleteID[0]];
            my_collection_deleteID.remove(0);
        }
        else
        {
            my_collection_ID.insert(my_collection_ID.begin(),My_collection->count()-1);      //下标是当前列表序列号,值是item的id值
            my_collection_id.push_back(My_collection->count()-1);      //下标是item的id值,值是所属的列表序列号
            w->setindex(My_collection->count()-1,item_widget::My_listwidget::My_collection_list,songname);
        }
        w->setlove(true);
        My_collection->setItemWidget(item,w);

        connect(w,&item_widget::item_widget_index,this,&Widget::slotitem_start);
        connect(w,&item_widget::item_widget_love,this,&Widget::slotitem_love);
    }

    else if(listname == My_listwidget::Recently_played_list) //最近播放
    {
        Recently_played->insertItem(0,item);
        item_widget *w = new item_widget(Recently_played);

        if(!recently_played_deleteID.isEmpty())       //id复用
        {
            recently_played_ID.insert(recently_played_ID.begin(),recently_played_deleteID[0]);
            recently_played_id[recently_played_deleteID[0]] = Recently_played->count()-1;
            w->setindex(recently_played_deleteID[0],item_widget::My_listwidget::Recently_played_list,songname);
            recently_played_deleteID.remove(0);
        }
        else
        {
            recently_played_ID.insert(recently_played_ID.begin(),Recently_played->count()-1);      //下标是当前列表序列号,值是item的id值
            recently_played_id.push_back(Recently_played->count()-1);      //下标是item的id值,值是所属的列表序列号
            w->setindex(Recently_played->count()-1,item_widget::My_listwidget::Recently_played_list,songname);
        }

        if(!My_collection_Array.isEmpty())     //判断当前歌曲是否已经收藏为我喜欢,判断我的收藏列表容器中是否有该歌曲序号
        {
            if(Recently_played_Array[0] == My_collection_Array[My_collection->count()-1 - my_collection_array[Recently_played_Array[0]]])
            {
                w->setlove(true);
            }
        }
        Recently_played->setItemWidget(item,w);

        connect(w,&item_widget::item_widget_index,this,&Widget::slotitem_start);
        connect(w,&item_widget::item_widget_love,this,&Widget::slotitem_love);
    }

}

//删除我的收藏,最近播放        id的值是歌曲的序列号  不对可能还要在申请一个容器
void Widget::delete_list(My_listwidget listname, int id,int item_id)
{
    int row;
    if(listname == My_listwidget::My_collection_list)
    {
        if(My_collection_Array.isEmpty())   return;

        row = My_collection->count()-1 - my_collection_array[id];

        //我喜欢恢复列表
        my_collection_delete_false.push_back(my_collection_array[id]);
        my_collection_deleteID.push_back(item_id);           //id复用
        qDebug() << my_collection_deleteID[my_collection_deleteID.size()-1];

        My_collection->setCurrentRow(row);    //移动到要删除的位置,

        if(My_collection->count()-1 - my_collection_array[id] != 0)
        {
        //删除行以前的所有列表号减一
            for(int i=row-1; i>=0; i--)
            {
                --my_collection_array[My_collection_Array[i]];
                if(my_collection_id[my_collection_ID[i]] == 0)    continue;
                --my_collection_id[my_collection_ID[i]];
            }
        }

        QWidget *w = My_collection->itemWidget(My_collection->currentItem());   //先分离item上面的控件然后依次消除
        w->disconnect(this);
        w->deleteLater();

        delete My_collection->takeItem(row);

        //item信号容器不能删除只能赋值为零,总数只能增多不能减少,因为在列表ID容器中item的id值还是存在的,还是会访问的
        //比如列表ID总数5个,删除第一个id值为0,如果item信号容器也跟着删除,那就无法使用id复用,并且原本的信号标记都会自动减一
        //再次点击item信号会产生信息不对称,报错等
        my_collection_id[my_collection_ID[row]] = 0;
        my_collection_ID.remove(row);
        My_collection_Array.remove(row);                     //在容器中销毁要删除的行
        my_collection_array[id] = 0;   //相应的歌曲序列号归零
    }

    else if(listname == My_listwidget::Recently_played_list)
    {
        if(Recently_played_Array.isEmpty())   return;

        row = Recently_played->count()-1 - recently_player_array[id];

        //我喜欢恢复列表
        my_collection_delete_false.push_back(my_collection_array[id]);
        recently_played_deleteID.push_back(item_id);           //id复用

        Recently_played->setCurrentRow(row);    //移动到要删除的位置,

        if(Recently_played->count()-1 - recently_player_array[id] != 0)
        {
        //删除行以前的所有列表号减一
            for(int i=row-1; i>=0; i--)
            {
                --recently_player_array[Recently_played_Array[i]];
                if(recently_played_id[recently_played_ID[i]] == 0)    continue;
                --recently_played_id[recently_played_ID[i]];
            }
        }

        QWidget *w = Recently_played->itemWidget(Recently_played->currentItem());   //先分离item上面的控件然后依次消除
        w->disconnect(this);
        w->deleteLater();

        delete Recently_played->takeItem(row);

        recently_player_array[id] = 0;    //清除歌曲数组中的行号
        Recently_played_Array.remove(row);                //清除列表容器的指定位置
        recently_played_deleteID.push_back(recently_played_ID[Recently_played_Array.size()-1]); //获取控件的id值
        recently_played_id[recently_played_ID[row]] = 0;     //要删除的id值为零,以便复用
        recently_played_ID.remove(row);                      //清除最后一行
    }
}

//统一item上面的暂停播放图标
void Widget::unified_item_play()
{
    int tem = playlist->currentIndex();
    playlist_val = tem;     //记录上一首的位置

    //统一item上面的暂停播放图标
    if(listwidget_Array[listwidget_array[tem]] == tem)
    {

        ui->listWidget->setCurrentRow(listwidget_array[tem]);
        QWidget *w = ui->listWidget->itemWidget(ui->listWidget->currentItem());
        QPushButton *bu = w->findChild<QPushButton *>("Pause_start_Button");
        //统一播放样式
        if(QMediaPlayer::PlayingState == player->state())
            bu->setText("开始");
        else if(QMediaPlayer::StoppedState == player->state() || QMediaPlayer::PausedState == player->state())
            bu->setText("暂停");
    }
    if(!Recently_played_Array.isEmpty())
    {
        if(Recently_played_Array[Recently_played->count()-1 - recently_player_array[tem]] == tem)
        {
            Recently_played->setCurrentRow(Recently_played->count()-1 - recently_player_array[tem]);
            QWidget *w = Recently_played->itemWidget(Recently_played->currentItem());
            QPushButton *bu = w->findChild<QPushButton *>("Pause_start_Button");
            //统一播放样式
            if(QMediaPlayer::PlayingState == player->state())
                bu->setText("开始");
            else if(QMediaPlayer::StoppedState == player->state() || QMediaPlayer::PausedState == player->state())
                bu->setText("暂停");
        }
    }
    if(!My_collection_Array.isEmpty())
    {
       if(My_collection_Array[My_collection->count()-1 - my_collection_array[tem]] == tem)
       {
           My_collection->setCurrentRow(My_collection->count()-1 - my_collection_array[tem]);
           QWidget *w = My_collection->itemWidget(My_collection->currentItem());
           QPushButton *bu = w->findChild<QPushButton *>("Pause_start_Button");
           //统一播放样式
           if(QMediaPlayer::PlayingState == player->state())
               bu->setText("开始");
           else if(QMediaPlayer::StoppedState == player->state() || QMediaPlayer::PausedState == player->state())
               bu->setText("暂停");
       }
    }
    if(!Answer_Array.isEmpty())
    {
        if(Answer_Array[answer_array[tem]] == tem)
        {
            Answer->setCurrentRow(answer_array[tem]);
            QWidget *w = Answer->itemWidget(Answer->currentItem());
            QPushButton *bu = w->findChild<QPushButton *>("Pause_start_Button");
            //统一播放样式
            if(QMediaPlayer::PlayingState == player->state())
                bu->setText("开始");
            else if(QMediaPlayer::StoppedState == player->state() || QMediaPlayer::PausedState == player->state())
                bu->setText("暂停");
        }
    }
}

//获取itenn上面的控件,然后获取歌名
QString Widget::item_name()
{
    if(list == My_listwidget::listwidget)
    {
        //获取当前item上面的控件
        QWidget *w = ui->listWidget->itemWidget(ui->listWidget->currentItem());
        //指定获取哪一种类型的控件  如果有多个控件可以指定获取::QLabel* la = w->findchild<QLabel *>("控件名称")
        QLabel *la = w->findChild<QLabel *>();
        ui->label_3->setText(la->text());
    }
    if(list == My_listwidget::My_collection_list)
    {
        QWidget *w = My_collection->itemWidget(My_collection->currentItem());
        QLabel *la = w->findChild<QLabel *>();
        ui->label_3->setText(la->text());
    }
    if(list == My_listwidget::Recently_played_list)
    {
        QWidget *w = Recently_played->itemWidget(Recently_played->currentItem());
        QLabel *la = w->findChild<QLabel *>();
        ui->label_3->setText(la->text());
    }
    if(list == My_listwidget::Answer_list)
    {
        QWidget *w = Answer->itemWidget(Answer->currentItem());
        QLabel *la = w->findChild<QLabel *>();
        ui->label_3->setText(la->text());
    }

    m_window.setname(ui->label_3->text());      //传递歌曲名
    return ui->label_3->text();
}

//隐藏listwidget
void Widget::listwidgethide()
{
    QListWidget *lw[] = {My_collection,Recently_played,Answer,ui->listWidget};
    for(int i=0; i<4; i++)
    {
        //隐藏并占位
        QSizePolicy sp = ui->Function->sizePolicy();
        sp.setRetainSizeWhenHidden(true);
        lw[i]->setSizePolicy(sp);
        //隐藏所有listwidget
        lw[i]->hide();
    }
}

//查重
bool Widget::Recently_played_Duplicate_checking(int id)
{
    if(!Recently_played_Array.isEmpty())
    {
        if(id == Recently_played_Array[Recently_played->count()-1 - recently_player_array[id]])  //如果歌曲已存在就修改歌曲当前状态并移动到该行
            return true;

        else if(Recently_played_Array.size()-1 == 8)     //只保存9首音乐
        {
            //删除函数,1.当前使用列表,2.要删除歌曲的序列号,3.item的id值以备id复用
            delete_list(My_listwidget::Recently_played_list,
                        Recently_played_Array[Recently_played_Array.size()-1],
                        recently_played_ID[Recently_played_Array.size()-1]);
        }
    }

    return false;
}

//随机播放,获取随机种子进行播放
void Widget::shuffle()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));      //申请随机数
    int tem = 0;

    if(list == My_listwidget::listwidget)
    {
        do  //获取不大于列表总值并且不等与当前行数的随机数
        {
            tem = qrand()%ui->listWidget->count();
        }while(tem == ui->listWidget->currentRow());

        ui->listWidget->setCurrentRow(tem);
        playlist->setCurrentIndex(listwidget_Array[tem]);
        listwidget_random.push_back(tem);       //记录当前随机的行号
    }
    if(list == My_listwidget::My_collection_list)
    {
        do
        {
            tem = qrand()%My_collection->count();
        }while(tem == My_collection->currentRow());

        My_collection->setCurrentRow(tem);
        playlist->setCurrentIndex(My_collection_Array[tem]);
        my_collection_random.push_back(tem);
    }
    if(list == My_listwidget::Recently_played_list)
    {
        do
        {
            tem = qrand()%Recently_played->count();
        }while(tem == Recently_played->currentRow());

        Recently_played->setCurrentRow(tem);
        playlist->setCurrentIndex(Recently_played_Array[tem]);
        recently_played_random.push_back(tem);
    }
    if(list == My_listwidget::Answer_list)
    {
        do
        {
            tem = qrand()%Answer->count();
        }while(tem == Answer->currentRow());

        Answer->setCurrentRow(tem);
        playlist->setCurrentIndex(Answer_Array[tem]);
        answer_random.push_back(tem);
    }
}

//计算拉伸区域Rect位置
void Widget::CalculateCurrentStrechRect()
{
    // 四个角Rect;
    m_leftTopRect = QRect(0, 0, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    //qDebug() << m_leftTopRect;
    m_leftBottomRect = QRect(0, this->height() - STRETCH_RECT_HEIGHT, STRETCH_RECT_WIDTH, STRETCH_RECT_WIDTH);
    //qDebug() << m_leftBottomRect;
    m_rightTopRect = QRect(this->width() - STRETCH_RECT_WIDTH, 0, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    //qDebug() << m_rightTopRect;
    m_rightBottomRect = QRect(this->width() - STRETCH_RECT_WIDTH, this->height() - STRETCH_RECT_HEIGHT, STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT);
    //qDebug() << m_rightBottomRect;

    // 四条边Rect;
    m_topBorderRect = QRect(STRETCH_RECT_WIDTH, 0, this->width() - STRETCH_RECT_WIDTH * 2, STRETCH_RECT_HEIGHT);
    m_rightBorderRect = QRect(this->width() - STRETCH_RECT_WIDTH, STRETCH_RECT_HEIGHT, STRETCH_RECT_WIDTH, this->height() - STRETCH_RECT_HEIGHT * 2);
    m_bottomBorderRect = QRect(STRETCH_RECT_WIDTH, this->height() - STRETCH_RECT_HEIGHT, this->width() - STRETCH_RECT_WIDTH * 2, STRETCH_RECT_HEIGHT);
    m_leftBorderRect = QRect(0, STRETCH_RECT_HEIGHT, STRETCH_RECT_WIDTH, this->height() - STRETCH_RECT_HEIGHT * 2);
}

//在窗口第一次show完之后需要计算拉伸区域rect位置
void Widget::showEvent(QShowEvent *)
{
    CalculateCurrentStrechRect();
}

//根据当前位置,定位鼠标在具体哪一块拉伸区域
WindowStretchRectState Widget::getCurrentStretchState(QPoint cursorPos)
{
    WindowStretchRectState stretchState;

    if (m_leftTopRect.contains(cursorPos))
    {
        stretchState = LEFT_TOP_RECT;
    }
    else if (m_rightTopRect.contains(cursorPos))
    {
        stretchState = RIGHT_TOP_RECT;
    }
    else if (m_rightBottomRect.contains(cursorPos))
    {
        stretchState = RIGHT_BOTTOM_RECT;
    }
    else if (m_leftBottomRect.contains(cursorPos))
    {
        stretchState = LEFT_BOTTOM_RECT;
    }
    else if (m_topBorderRect.contains(cursorPos))
    {
        stretchState = TOP_BORDER;
    }
    else if (m_rightBorderRect.contains(cursorPos))
    {
        stretchState = RIGHT_BORDER;
    }
    else if (m_bottomBorderRect.contains(cursorPos))
    {
        stretchState = BOTTOM_BORDER;
    }
    else if (m_leftBorderRect.contains(cursorPos))
    {
        stretchState = LEFT_BORDER;
    }
    else
    {
        stretchState = NO_SELECT;
    }

    return stretchState;

}

//根据返回的拉伸区域状态进行更新鼠标样式
void Widget::updateMouseStyle(WindowStretchRectState stretchState)
{
    switch(stretchState)
    {
    case NO_SELECT:
        setCursor(Qt::ArrowCursor);
        break;
    case LEFT_TOP_RECT:
    case RIGHT_BOTTOM_RECT:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case TOP_BORDER:
    case BOTTOM_BORDER:
        setCursor(Qt::SizeVerCursor);
        break;
    case RIGHT_TOP_RECT:
    case LEFT_BOTTOM_RECT:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case LEFT_BORDER:
    case RIGHT_BORDER:
        setCursor(Qt::SizeHorCursor);
        break;
    default:
        setCursor(Qt::ArrowCursor);
        break;

    }
}

//计算窗口与鼠标位置的差值(重写鼠标单机)
void Widget::mousePressEvent(QMouseEvent *ev)
{
    //开启计时器
    m_cTimer.start(200);

    //判断是否鼠标左键按下
    if(ev->buttons() == Qt::LeftButton)
    {
        //如果鼠标样式不等于0
        if(stretchState != NO_SELECT)
        {
            //记录当前鼠标位置,为后面计算拉伸位置
            m_startPoint = this->mapToGlobal(ev->pos());
            //保存拉伸前的窗口位置及大小
            window_Beginning = this->geometry();
        }
        //计算窗口与鼠标位置的差值
        Diff_val = ev->globalPos() - this->pos();
    }
}

//窗口移动重写函数
void Widget::mouseMoveEvent(QMouseEvent *ev)
{
    //判断计时器是否在工作,如果是就退出
    if(m_cTimer.isActive()) return;
    //判断是否最大化,最大化是不可拉伸和移动的
    if(windowState() == Qt::WindowMaximized)
        return;

    //如果按钮在没有按下的情况下移动那就返回它的位置
    if(ev->buttons() == Qt::NoButton)
    {
        QPoint cursorPOs = ev->pos();
        //根据当前鼠标的位置更新鼠标的状态及样式
        stretchState = getCurrentStretchState(cursorPOs);
        updateMouseStyle(stretchState);
    }
    if(ev->buttons() == Qt::LeftButton)
    {
        if(cursor() == Qt::ArrowCursor)
        {
            //鼠标位置减去差值就是窗口移动的位置
            this->move(ev->globalPos() - Diff_val);
            return;
        }
        m_endPoint = this->mapToGlobal(ev->pos());
        updateWindowSize();
    }
}

//重写鼠标释放事件
void Widget::mouseReleaseEvent(QMouseEvent *)
{
    //判断计时器是否在工作
    if(m_cTimer.isActive()) return;
    //重新计算用于拉伸区域的rect
    CalculateCurrentStrechRect();
}

//拉伸过程中,根据记录的坐标更新窗口大小
void Widget::updateWindowSize()
{
    //获取还未拉伸前的窗口位置及大小
    QRect windowRect = window_Beginning;
    //获取要增长多少的x和y
    //以鼠标点击为中心,往左拉伸数值越小,往右拉伸数值越大;
    //起始点减去终止点得出要拉伸的值,
    int delValue_X = m_startPoint.x() - m_endPoint.x();
    int delValue_Y = m_startPoint.y() - m_endPoint.y();
    //qDebug() << "m_startPoint =" << m_startPoint;
    qDebug() << "m_endPoint =" << m_endPoint;


    if(stretchState == LEFT_BORDER)             //左边
    {
        //获取鼠标点击左上角的值,用该值减去要拉伸的值,如果拉伸的值为负数刚好负负为正
        QPoint topLeftPoint = windowRect.topLeft();
        //qDebug() << "top =" << topLeftPoint;
        topLeftPoint.setX(topLeftPoint.x() - delValue_X);
        windowRect.setTopLeft(topLeftPoint);
    }
    else if (stretchState == RIGHT_BORDER)       //右边
    {
        QPoint bottomRightPoint = windowRect.bottomRight();
        bottomRightPoint.setX(bottomRightPoint.x() - delValue_X);
        windowRect.setBottomRight(bottomRightPoint);
    }
    else if (stretchState == TOP_BORDER)         //顶端
    {
        QPoint topLeftPoint = windowRect.topLeft();
        topLeftPoint.setY(topLeftPoint.y() - delValue_Y);
        windowRect.setTopLeft(topLeftPoint);
    }
    else if (stretchState == BOTTOM_BORDER)         //底端
    {
        QPoint bottomRightPoint = windowRect.bottomRight();
        bottomRightPoint.setY(bottomRightPoint.y() - delValue_Y);
        windowRect.setBottomRight(bottomRightPoint);
    }
    else if (stretchState == LEFT_TOP_RECT)         //左上角
    {
        QPoint topLeftPoint = windowRect.topLeft();
        topLeftPoint.setX(topLeftPoint.x() - delValue_X);
        topLeftPoint.setY(topLeftPoint.y() - delValue_Y);
        windowRect.setTopLeft(topLeftPoint);
    }
    else if (stretchState == RIGHT_TOP_RECT)         //右上角
    {
        QPoint topRightPoint = windowRect.topRight();
        topRightPoint.setX(topRightPoint.x() - delValue_X);
        topRightPoint.setY(topRightPoint.y() - delValue_Y);
        windowRect.setTopRight(topRightPoint);
    }
    else if (stretchState == RIGHT_BOTTOM_RECT)      //右下角
    {
        QPoint bottomRightPoint = windowRect.bottomRight();
        bottomRightPoint.setX(bottomRightPoint.x() - delValue_X);
        bottomRightPoint.setY(bottomRightPoint.y() - delValue_Y);
        windowRect.setBottomRight(bottomRightPoint);
    }
    else if (stretchState == LEFT_BOTTOM_RECT)      //左下角
    {
        QPoint bottomLeftPoint = windowRect.bottomLeft();
        bottomLeftPoint.setX(bottomLeftPoint.x() - delValue_X);
        bottomLeftPoint.setY(bottomLeftPoint.y() - delValue_Y);
        windowRect.setBottomLeft(bottomLeftPoint);
    }

    this->setGeometry(windowRect);
}

//重新绘制背景图
void Widget::paintEvent(QPaintEvent *)
{
    QPainter pain(this);
    pain.drawPixmap(rect(),QPixmap("C:/Users/HP/Desktop/image/jt_1.webp"),QRect());
}

//重写界面大小事件
void Widget::resizeEvent(QResizeEvent *)
{
    imglabel->resize(this->size());
    My_collection->resize(ui->listWidget->size());
    Recently_played->resize(ui->listWidget->size());
    Answer->resize(ui->listWidget->size());

}

//鼠标双击最大化/还原窗口
void Widget::mouseDoubleClickEvent(QMouseEvent *)
{
    //获取当前窗口状态并判断是否最大化
    if(this->windowState() == Qt::WindowMaximized)
    {
        this->setWindowState(Qt::WindowNoState);
    }
    else
        this->setWindowState(Qt::WindowMaximized);
}

//计时器超时停止计时
void Widget::slotTimerTimerOut()
{
    m_cTimer.stop();
}

//小窗口关闭,窗口还原
void Widget::slotReduction()
{
    this->show();
}

//点击暂停播放按钮
void Widget::on_pushButton_clicked()
{
    int state = player->state();

    //当直接点击播放按钮时，无法捕捉到音乐的名字所以要修改一下
    if(ui->listWidget->currentRow() <= 0)
    {
        ui->listWidget->setCurrentRow(0);
        ui->label_3->setText(ui->listWidget->currentItem()->text());
        m_window.setname(ui->label_3->text());      //传递歌名
    }

    switch(state)
    {
        case QMediaPlayer::StoppedState:
             player->play();
             ui->pushButton->setText("开始");
             break;
        case QMediaPlayer::PlayingState:
             player->pause();
             //ui->pushButton->setStyleSheet("border-image: url(C:/Users/HP/Desktop/image/stop.png)");
             ui->pushButton->setText("暂停");
             break;
        case QMediaPlayer::PausedState:
             player->play();
             ui->pushButton->setText("开始");
             break;
    }

    unified_item_play();    //统一item的图标
}

//上一首
void Widget::on_pushButton_2_clicked()
{
    //顺序,单曲正常加减一,随机需要获取上一首的索引,判断当前列表是否有该索引
    if(list == My_listwidget::listwidget)
    {
        if(QMediaPlaylist::Random == playlist->playbackMode())      //随机
        {
            if(listwidget_random.isEmpty() == true || listwidget_random.size() == 1)    //有点问题
            {
                shuffle();
            }
            else
            {
                //修改playlist音乐播放列表的位置为上一次随机播放的位置
                playlist->setCurrentIndex(listwidget_Array[listwidget_random[listwidget_random.size()-1]]);
                //修改listwidget列表的位置为上一次随机播放的位置
                ui->listWidget->setCurrentRow(listwidget_random[listwidget_random.size()-1]);
                //每次进行上次播放就把记录随机播放容器的最后一个值删除
                listwidget_random.remove(listwidget_random.size()-1);
            }
        }
        else if(ui->listWidget->currentRow() < 0)   //如果直接点击上一首就播放第一首音乐
        {
            playlist->setCurrentIndex(listwidget_Array[0]);
            ui->listWidget->setCurrentRow(0);
        }
        else if(ui->listWidget->currentRow() > 0)   //如果歌曲正在播放点击上一首就播放上一首
        {
            //获取当前播放音乐在音乐列表的位置在由歌曲序号数组来定位该歌曲在列表中的位置
            ui->listWidget->setCurrentRow(listwidget_array[playlist->currentIndex()]-1);
            playlist->setCurrentIndex(listwidget_Array[ui->listWidget->currentRow()]);
        }
        else if(listwidget_array[playlist->currentIndex()] == 0)  //如果正在播放的是列表的第一首歌曲,那下一次播放的因该是本列表的最后一首歌曲
        {
            playlist->setCurrentIndex(listwidget_Array[ui->listWidget->count()-1]);
            ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
        }
    }

    else if(list == My_listwidget::My_collection_list)  //我的收藏
    {
        if(QMediaPlaylist::Random == playlist->playbackMode())      //随机
        {
            if(my_collection_random.isEmpty() == true || my_collection_random.size() == 1)
            {
                shuffle();
            }
            else
            {
                playlist->setCurrentIndex(My_collection_Array[my_collection_random[my_collection_random.size()-1]]);
                My_collection->setCurrentRow(my_collection_random[my_collection_random.size()-1]);
                my_collection_random.remove(my_collection_random.size()-1);
            }
        }
        else if(My_collection->currentRow() < 0)
        {
            playlist->setCurrentIndex(My_collection_Array[0]);
            My_collection->setCurrentRow(0);
        }
        else if(My_collection->currentRow() > 0)
        {
            //因为是每次把歌曲加到第一行,所以第一首歌曲在最后面,列表总数减去列表号在减一;
            My_collection->setCurrentRow((My_collection->count()-1 - my_collection_array[playlist->currentIndex()]) -1);
            playlist->setCurrentIndex(My_collection_Array[My_collection->currentRow()]);

        }
        else if(My_collection->count()-1 - my_collection_array[playlist->currentIndex()] == 0)
        {
            playlist->setCurrentIndex(My_collection_Array[My_collection->count()-1]);
            My_collection->setCurrentRow(My_collection->count()-1);
        }
    }

    else if(list == My_listwidget::Recently_played_list)    //最近播放
    {
        if(QMediaPlaylist::Random == playlist->playbackMode())      //随机
        {
            if(recently_played_random.isEmpty() == true || recently_played_random.size() == 1)
            {
                shuffle();
            }
            else
            {
                playlist->setCurrentIndex(Recently_played_Array[recently_played_random[recently_played_random.size()-1]]);
                Recently_played->setCurrentRow(recently_played_random[recently_played_random.size()-1]);
                recently_played_random.remove(recently_played_random.size()-1);
            }
        }
        else if(Recently_played->currentRow() < 0)
        {
            playlist->setCurrentIndex(Recently_played_Array[0]);
            Recently_played->setCurrentRow(0);
        }
        else if(Recently_played->currentRow() > 0)
        {
            Recently_played->setCurrentRow((Recently_played->count()-1 - recently_player_array[playlist->currentIndex()]) -1);
            playlist->setCurrentIndex(Recently_played_Array[Recently_played->currentRow()]);
        }
        else if(Recently_played->count()-1 - recently_player_array[playlist->currentIndex()] == 0)
        {
            playlist->setCurrentIndex(Recently_played_Array[Recently_played->count()-1]);
            Recently_played->setCurrentRow(Recently_played->count()-1);
        }
    }

    else if(list == My_listwidget::Answer_list) //搜索
    {
        if(QMediaPlaylist::Random == playlist->playbackMode())      //随机
        {
            if(answer_random.isEmpty() == true || answer_random.size() == 1)
            {
                shuffle();
            }
            else
            {
                playlist->setCurrentIndex(Answer_Array[answer_random[answer_random.size()-1]]);
                Answer->setCurrentRow(answer_random[answer_random.size()-1]);
                answer_random.remove(answer_random.size()-1);
            }
        }
        else if(Answer->currentRow() < 0)
        {
            playlist->setCurrentIndex(Answer_Array[0]);
            Answer->setCurrentRow(0);
        }
        else if(Answer->currentRow() > 0)
        {
            Answer->setCurrentRow(answer_array[playlist->currentIndex()]-1);
            playlist->setCurrentIndex(Answer_Array[Answer->currentRow()]);
        }
        else if(answer_array[playlist->currentIndex()] == 0)
        {
            playlist->setCurrentIndex(Answer_Array[Answer->count()-1]);
            Answer->setCurrentRow(Answer->count()-1);
        }
    }

    item_name();
    player->play();
}

//下一首
void Widget::on_pushButton_3_clicked()
{
    if(list == My_listwidget::listwidget)
    {
        if(QMediaPlaylist::Random == playlist->playbackMode())      //随机
        {
            shuffle();
        }
        else if(ui->listWidget->currentRow() < 0)
        {
            playlist->setCurrentIndex(listwidget_Array[0]);
            ui->listWidget->setCurrentRow(0);
        }
        //所选行大于等于零且当前播放音乐行小于列表总数时
        else if(ui->listWidget->currentRow() >= 0 && listwidget_array[playlist->currentIndex()] < ui->listWidget->count()-1)
        {
            ui->listWidget->setCurrentRow(listwidget_array[playlist->currentIndex()]+1);
            playlist->setCurrentIndex(listwidget_Array[ui->listWidget->currentRow()]);
        }
        else if(listwidget_array[playlist->currentIndex()] == ui->listWidget->count()-1)
        {
            playlist->setCurrentIndex(listwidget_Array[0]);
            ui->listWidget->setCurrentRow(0);
        }
    }

    else if(list == My_listwidget::My_collection_list)  //我的收藏
    {
        if(QMediaPlaylist::Random == playlist->playbackMode())      //随机
        {
            shuffle();
        }
        else if(My_collection->currentRow() < 0)
        {
            playlist->setCurrentIndex(My_collection_Array[0]);
            My_collection->setCurrentRow(0);
        }
        else if(My_collection->currentRow() >= 0 &&
                My_collection->count()-1 - my_collection_array[playlist->currentIndex()] < My_collection->count()-1)
        {
            //因为是每次把歌曲加到第一行,所以第一首歌曲在最后面,列表总数减去列表号在加一;
            My_collection->setCurrentRow((My_collection->count()-1 - my_collection_array[playlist->currentIndex()]) +1);
            playlist->setCurrentIndex(My_collection_Array[My_collection->currentRow()]);
        }
        else if(My_collection->count()-1 - my_collection_array[playlist->currentIndex()] == My_collection->count()-1)
        {
            playlist->setCurrentIndex(My_collection_Array[0]);
            My_collection->setCurrentRow(0);
        }
    }

    else if(list == My_listwidget::Recently_played_list)  //最近播放
    {
        if(QMediaPlaylist::Random == playlist->playbackMode())      //随机
        {
            shuffle();
        }
        else if(Recently_played->currentRow() < 0)
        {
            playlist->setCurrentIndex(Recently_played_Array[0]);
            Recently_played->setCurrentRow(0);
        }
        else if(Recently_played->currentRow() >= 0 && Recently_played->count()-1 - recently_player_array[playlist->currentIndex()] < Recently_played->count()-1)
        {
            Recently_played->setCurrentRow(Recently_played->count()-1 - recently_player_array[playlist->currentIndex()]+1);
            playlist->setCurrentIndex(Recently_played_Array[Recently_played->currentRow()]);
        }
        else if(Recently_played->count()-1 - recently_player_array[playlist->currentIndex()] == Recently_played->count()-1)
        {
            playlist->setCurrentIndex(Recently_played_Array[0]);
            Recently_played->setCurrentRow(0);
        }
    }

    else if(list == My_listwidget::Answer_list)  //搜索
    {
        if(QMediaPlaylist::Random == playlist->playbackMode())      //随机
        {
            shuffle();
        }
        else if(Answer->currentRow() < 0)
        {
            playlist->setCurrentIndex(Answer_Array[0]);
            Answer->setCurrentRow(0);
        }
        else if(Answer->currentRow() >= 0 && answer_array[playlist->currentIndex()] < Answer->count()-1)
        {
            Answer->setCurrentRow(answer_array[playlist->currentIndex()]+1);
            playlist->setCurrentIndex(Answer_Array[Answer->currentRow()]);
        }
        else if(answer_array[playlist->currentIndex()] >= Answer->count()-1)
        {
            playlist->setCurrentIndex(Answer_Array[0]);
            Answer->setCurrentRow(0);
        }
    }
    item_name();
    player->play();
}

//搜索按钮
void Widget::on_pushButton_4_clicked()
{
    QString tmp = ui->lineEdit->text();     //获取搜索框内的文本内容
    if(tmp == "")   return;

    //清空列表,列表容器和歌曲序列号数组
    Answer->clear();
    memset(answer_array,0,sizeof(answer_array));
    Answer_Array.clear();

    //大小位置已经预留啦,多获取一次大小就好啦
    Answer->resize(ui->listWidget->size());     //修改大小
    Answer->move(ui->listWidget->pos());        //移动位置

    //隐藏并占位
    listwidgethide();
    Answer->show();

    //遍历总的歌名容器，并判断搜索框内的文本是否为它的子串
    for(int i=0; i<total_song_name_Array.size(); i++)
    {
        if(total_song_name_Array[i].contains(tmp))
        {
            Answer_Array.push_back(total_song_val_array[i]);    //列表行号对应歌曲序列号
            answer_array[i] = Answer->count();          //answer歌曲序列表对应列表行号

            //初始化列表框中小列表项
            QListWidgetItem *item = new QListWidgetItem(Answer);
            item->setSizeHint(QSize(100,60));
             item_widget *w = new item_widget(Answer);
             //answer->count()-1    因为上面定义的item已经使得本列表的总值变为一啦
             w->setindex(Answer->count()-1,item_widget::My_listwidget::Answer_list,total_song_name_Array[i]);

             if(!My_collection_Array.isEmpty())     //判断搜索的结果是否在我的收藏中
             {
                 if(i == My_collection_Array[My_collection->count()-1 - my_collection_array[i]])
                 {
                     w->setlove(true);
                 }
             }
            Answer->setItemWidget(item,w);

            connect(w,&item_widget::item_widget_index,this,&Widget::slotitem_start);
            connect(w,&item_widget::item_widget_love,this,&Widget::slotitem_love);
        }
    }
}

//设置列表播放模式
void Widget::on_pushButton_5_clicked()
{
    //playlist->playbackMode()::获取当前的播放模式
    if(QMediaPlaylist::Loop == playlist->playbackMode())
    {
        playlist->setPlaybackMode(QMediaPlaylist::Random);
        ui->pushButton_5->setText("随机");
        return;
    }

    if(QMediaPlaylist::Random == playlist->playbackMode())
    {
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        ui->pushButton_5->setText("单曲");
        return;
    }
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

#if 0
        if(QMediaPlaylist::Loop == playlist->playbackMode() ||
           QMediaPlaylist::Loop == My_collection_playlist->playbackMode() ||
           QMediaPlaylist::Loop == Recently_playlist->playbackMode() ||
           QMediaPlaylist::Loop == Answer_playlist->playbackMode())
        {
            playlist->setPlaybackMode(QMediaPlaylist::Random);
            My_collection_playlist->setPlaybackMode(QMediaPlaylist::Random);
            Recently_playlist->setPlaybackMode(QMediaPlaylist::Random);
            Answer_playlist->setPlaybackMode(QMediaPlaylist::Random);
            ui->pushButton_5->setText("随机");
            return;
        }

        if(QMediaPlaylist::Random == playlist->playbackMode() ||
           QMediaPlaylist::Random == My_collection_playlist->playbackMode()||
           QMediaPlaylist::Random == Recently_playlist->playbackMode() ||
           QMediaPlaylist::Random == Answer_playlist->playbackMode())
        {
            playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
            My_collection_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
            Recently_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
            Answer_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
            ui->pushButton_5->setText("单曲");
            return;
        }

        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        My_collection_playlist->setPlaybackMode(QMediaPlaylist::Loop);
        Recently_playlist->setPlaybackMode(QMediaPlaylist::Loop);
        Answer_playlist->setPlaybackMode(QMediaPlaylist::Loop);
#endif

        ui->pushButton_5->setText("循环");
        return;
}

//设置音量控件器显示与隐藏
void Widget::on_push_volume_clicked()
{
    if(lider_bool == false)
    {
        //显示部件
        lider->show();
        //移动部件至指定位置
        lider->move(ui->push_volume->pos().rx(),ui->push_volume->pos().ry() - 121);
        lider_bool = true;
    }
    else
    {
        lider->hide();
        lider_bool = false;
        return;
    }
}

//使窗口最小化
void Widget::on_min_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

//使窗口最大化/还原窗口
void Widget::on_max_clicked()
{
    if(this->windowState() == Qt::WindowMaximized)
        this->setWindowState(Qt::WindowNoState);
    else
        this->setWindowState(Qt::WindowMaximized);
}

//关闭窗口
void Widget::on_close_clicked()
{
    this->close();
}

//隐藏主窗口,显示小窗口
void Widget::on_pushButton_6_clicked()
{
    this->hide();
    m_window.show();
}
