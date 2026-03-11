#ifndef PLAYWIDGET_H
#define PLAYWIDGET_H
#include <QWidget>
#include <QPainter>
#include <QPushButton>
#include <QIcon>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout> //水平布局
#include <QVBoxLayout> //垂直布局
#include <QGridLayout> //网格布局
#include <QFrame>
#include <QFile>
#include <QTimer>//定时器
#include <QDateTime>//时间处理头文件
#include <QString>
#include <QDebug>
#include <QList>
#include <QGridLayout>
#include "view/loginwidget.h"
#include "view/monitorwidget.h"
#include "view/logwidget.h"
#include "view/playbackwidget.h"
#include "view/syssettingwidget.h"
#include <QListWidget>
#include <QStackedWidget>
#include <QEvent>
#include <QCloseEvent>
#include <QMouseEvent>
#include "thread/camerathread.h"
extern "C"
{
    #include "libavcodec/avcodec.h"            //编解码库，最重要的模块之一
    #include "libavdevice/avdevice.h"            //输入输出设备库；读取摄像头的
    #include "libavfilter/avfilter.h"            //音视频滤镜库；进行音视频处理与编辑
    #include "libavformat/avformat.h"            //文件格式和协议库
    #include "libavutil/avutil.h"                //音视频处理
    #include "libswresample/swresample.h"    //音频重采样
    #include "libswscale/swscale.h"           //图像进行格式转换
}
class PlayWidget : public QWidget
{
    Q_OBJECT

public:
    PlayWidget(QWidget *parent = 0);
    ~PlayWidget();
    void initUI();
    void initWin();
    void initQSS();
    void initconnect();
    void startCamera(const QString& cameraName,const QString& videoPath,const QString& photoPath,const int& videoCollectTimes);
    bool checkConfiguration();
    void setCurrentWidget(int widgetId);
    void clearCacheFile();
signals:
    void newVideoBuild(const QString& videoName,const QString& coverName,const int& framesCount);
protected:
    void closeEvent(QCloseEvent* event)override;

public slots:
    void updatetime();
    void updatewelcomename(QString userName);
    void onStartCamera();
    void showplaywidget();
    void onMonitorBtnClicked();
    void onPlaybackBtnClicked();
    void onLogBtnClicked();
    void onSettingBtnClicked();
    void onLoginBtnClicked();
    void onNewVideoBuild(const QString& videoName,const QString& coverName,const int& framesCount);


signals:
    void cameraOpened(const QString& cameraNames,const QString& videoPath,const QString& photoPath,const int& videoCollectTimes);//发送给monitorWidget

private:
    CameraThread* m_cameraThread[4];
    QWidget *topWidget;             //顶部窗口，用于切换不同模式和显示时间
    QStackedWidget *stackedWidget;   //底部栈窗口，用于显示当前的模式
    MonitorWidget* monitorWidget;   //监控窗口
    PlayBackWidget* playBackWidget; //回放窗口
    LogWidget* logWidget;           //日志查询
    SysSettingWidget* sysSettingWidget;  //系统设置窗口
    LoginWidget* loginWidget;       //登录窗口
    QLabel* logoLabel,*titleLabel,*welcomeLabel,*timeLabel;
    QPushButton *monitorBtn,*playbackBtn,*logBtn,*settingBtn,*loginBtn;
    QHBoxLayout *toplayout;
    QVBoxLayout *totalLayout;
    QTimer *timetimer;

    int currentWidgetId;
    bool isLogined;
};

#endif // PLAYWIDGET_H
