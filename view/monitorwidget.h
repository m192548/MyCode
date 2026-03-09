#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include <QObject>
#include <QWidget>
#include <QListWidget>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QEvent>
#include <QList>
#include "thread/camerathread.h"
#include <QTimer>
class MonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MonitorWidget(QWidget *parent = nullptr);
    void initUi();
    void initConnect();
    void initWin();
    void initQSS();
    //检测标签点击
    bool eventFilter(QObject *watched, QEvent *event);
    void showSingleChannel();
    void startTimer();

    void setVideoPath(const QString& path);
    void setPhotoPath(const QString& path);
    void setCameraNames(QList<QString>& cameraNames);
    void setCollectionTimes(const int& times);
signals:
    void newVideoBuild(const QString& videoName,const QString& coverName,const int& framesCount);
public slots:
    void onSingelChannelBtnClicked();
    void onFourChannelBtnClicked();
    void onCatchBtnClicked();
    void onStartCamera(const QString& cameraNames,const QString& videoPath,const QString& photoPath,const int& videoCollectTimes);
    void onStopCamera();
    void onImageReady(const QImage& img,bool isCover,const QString& coverFilename);
    void onErrorOccurred(const QString& errMsg);
    void onNewVideoBuild(const QString& videoName,const QString& coverName,const int& framesCount);
private:
    QWidget* leftWidgt;           //左侧窗口
    QListWidget* cameraListWidget;//左侧的设备列表窗口
    QWidget* VideoWidget;         //显示4个摄像头的窗口
    QWidget* bottomWidget;        //底部功能窗口

    //布局
    QHBoxLayout* containerHLayout;      //总布局
    QVBoxLayout* leftVLayout;           //左侧窗口布局
    QVBoxLayout* vLayout;               //右侧窗口布局
    QHBoxLayout* bottomHLayout;         //底部窗口布局
    QGridLayout* videoGridLayout;       //网格布局

    //标签
    QLabel* cameraLabel;
    QLabel *videolabel1,*videolabel2,*videolabel3,*videolabel4;

    //底部的功能按钮(单通道,四通道,拍照)
    QPushButton* singelChannelBtn,*fourChannelBtn,*catchBtn;

    int currentChannel; //当前通道id
    bool isSingleChannel;//当前是否为单通道

    CameraThread* cameraThread[4];
    QList<QString> cameraNames;
    QString currentCamera;
    QString videoPath;
    QString photoPath;
    int collectionTimes;
    bool cameraIsOpened;
};

#endif // MONITORWIDGET_H
