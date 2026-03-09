#include "playwidget.h"
#include <QQueue>
#include <QApplication>
PlayWidget::PlayWidget(QWidget *parent)
    : QWidget(parent)
{
    //定时器初始化
    this->timetimer=new QTimer(this);
    this->timetimer->start(1000);//1s刷新1次

    //登录界面初始化
    isLogined=false;
    this->loginWidget=new LoginWidget(nullptr);//子窗口的显示状态会依赖于父窗口,创建LoginWidget时不指定父对象（或指定为nullptr），使其成为独立窗口
    this->loginWidget->hide();
    this->initUI();
    this->initWin();
    this->initQSS();
    this->initconnect();



}

PlayWidget::~PlayWidget()
{
    if (timetimer) {
           timetimer->stop();
           delete timetimer;//清除定时器
           qDebug()<<"delete timetimer success!";
       }
    if(loginWidget)
    {
        loginWidget->deleteLater();
        loginWidget=nullptr;
    }
}

void PlayWidget::initUI()
{
    this->totalLayout=new QVBoxLayout(this);//顶级布局
    totalLayout->setSpacing(0);
    totalLayout->setMargin(0);
    //顶部布局容器
    this->topWidget=new QWidget(this);//装有logo,按钮,标签
    topWidget->setObjectName("topWidget");
    this->topWidget->setFixedHeight(100);//设置顶部固定高度
    this->topWidget->setStyleSheet("background-color: white");
    //顶部水平布局
    this->toplayout=new QHBoxLayout(topWidget);//布局属于topwidget
    this->toplayout->setSpacing(30);//每个元素间距
    //logo标签
    this->logoLabel=new QLabel(topWidget);
    this->logoLabel->setPixmap(QPixmap(":/images/monitor1.png").scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //标题标签
    this->titleLabel=new QLabel("智能安防监控系统",topWidget);//将标签存入父窗口
    this->titleLabel->setObjectName("playTitle");
    //监控模式按钮
    this->monitorBtn=new QPushButton("监控模式",topWidget);
    this->monitorBtn->setObjectName("monitorBtn");
    //回放模式按钮
    this->playbackBtn=new QPushButton("回放模式",topWidget);
    this->playbackBtn->setObjectName("playbackBtn");
    //日志查询按钮
    this->logBtn=new QPushButton("日志查询",topWidget);
    this->logBtn->setObjectName("logBtn");
    //系统设置按钮
    this->settingBtn=new QPushButton("系统设置",topWidget);
    this->settingBtn->setObjectName("settingBtn");
    //登录按钮
    this->loginBtn=new QPushButton("登录",topWidget);
    this->loginBtn->setObjectName("loginBtn");
    //欢迎标签
    this->welcomeLabel=new QLabel("欢迎!XXX",topWidget);
    this->welcomeLabel->setObjectName("welcomeLab");
    //系统时间标签
    this->timeLabel=new QLabel(topWidget);
    this->timeLabel->setObjectName("timeLab");
    this->toplayout->addWidget(logoLabel);// 添加logo标签,在topwidget中按照 toplayout 的水平布局规则
    this->toplayout->addWidget(titleLabel);
    this->toplayout->addWidget(monitorBtn);
    this->toplayout->addWidget(playbackBtn);
    this->toplayout->addWidget(logBtn);
    this->toplayout->addWidget(settingBtn);
    this->toplayout->addWidget(loginBtn);
    this->toplayout->addWidget(welcomeLabel);
    this->toplayout->addWidget(timeLabel);
    this->toplayout->addStretch();
    //创建StackedWidget
    stackedWidget=new QStackedWidget(this);
    stackedWidget->setObjectName("stackedWidget");
    //创建四个窗口
    monitorWidget=new MonitorWidget(stackedWidget);
    playBackWidget=new PlayBackWidget(stackedWidget);
    logWidget=new LogWidget(stackedWidget);
    sysSettingWidget=new SysSettingWidget(stackedWidget);
    //把四个窗口添加到satckedWidget中
    stackedWidget->addWidget(monitorWidget);
    stackedWidget->addWidget(playBackWidget);
    stackedWidget->addWidget(logWidget);
    stackedWidget->addWidget(sysSettingWidget);

    //设置当前窗口为监控窗口
    stackedWidget->setCurrentWidget(monitorWidget);

    //把底部窗口和stackedWidget添加到总布局
    this->totalLayout->addWidget(topWidget);
    this->totalLayout->addWidget(stackedWidget);
    //this->totallayout->addStretch();

}

void PlayWidget::initWin()
{
    this->setWindowTitle("安防系统界面");
    this->resize(800,600);
    this->setWindowIcon(QIcon(":/images/icon.png"));
}

void PlayWidget::initQSS()
{
    //加载外部qss文件
    QFile file(":/qss/playwidget.qss");//资源文件路径
    if(file.open(QFile::ReadOnly))
    {
        QString styleSheet=QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

}

void PlayWidget::initconnect()
{
    connect(timetimer, SIGNAL(timeout()), this, SLOT(updatetime()));
    connect(monitorBtn,SIGNAL(clicked()),this,SLOT(onMonitorBtnClicked()));
    connect(playbackBtn,SIGNAL(clicked()),this,SLOT(onPlaybackBtnClicked()));
    connect(logBtn,SIGNAL(clicked()),this,SLOT(onLogBtnClicked()));
    connect(settingBtn,SIGNAL(clicked()),this,SLOT(onSettingBtnClicked()));
    connect(loginBtn,SIGNAL(clicked()),this,SLOT(onLoginBtnClicked()));
    connect(loginWidget,SIGNAL(loginsuccess(QString)),this,SLOT(updatewelcomename(QString)));
    connect(loginWidget,SIGNAL(loginsuccess(QString)),this,SLOT(showplaywidget()));
    connect(loginWidget, SIGNAL(returntoplay()),this, SLOT(showplaywidget()));
    connect(sysSettingWidget,SIGNAL(configcompleted()),this,SLOT(onStartCamera()));
    connect(this,SIGNAL(cameraOpened(const QString&,const QString&,const QString&,const int&)),monitorWidget,SLOT(onStartCamera(const QString&,const QString&,const QString&,const int&)));
    connect(monitorWidget,SIGNAL(newVideoBuild(const QString&,const QString&,const int&)),this,SLOT(onNewVideoBuild(const QString&,const QString&,const int&)));
    connect(this,SIGNAL(newVideoBuild(const QString&,const QString&,const int&)),playBackWidget,SLOT(onNewVideoBuild(const QString&,const QString&,const int&)));
}
void PlayWidget::setCurrentWidget(int widgetId)
{
     currentWidgetId=widgetId;
    if(widgetId==3)
    {
        monitorBtn->setEnabled(false);
        playbackBtn->setEnabled(false);
        logBtn->setEnabled(false);
        stackedWidget->setCurrentWidget(sysSettingWidget);
    }
    if(widgetId==0)stackedWidget->setCurrentWidget(monitorWidget);
}


void PlayWidget::startCamera(const QString& cameraName,const QString& videoPath,const QString& photoPath,const int& videoCollectTimes)
{
    //发给monitorWidget
   emit cameraOpened(cameraName,videoPath,photoPath,videoCollectTimes);
}
bool PlayWidget::checkConfiguration()
{
    if(this->sysSettingWidget->checkconfig())
    {
        clearCacheFile();//清空当前目录的缓存
        QList<QString> cameraNames=sysSettingWidget->getCameraNames();
        monitorWidget->setCameraNames(sysSettingWidget->getCameraNames());
        return true;
    }
    return false;
}
void PlayWidget::updatetime()
{   //获取当前时间并格式化 (年-月-日 时:分:秒)
    QDateTime currentTime=QDateTime::currentDateTime();
    QString currenttimeStr=currentTime.toString("yyyy-MM-dd HH:mm:ss");
    this->timeLabel->setText(currenttimeStr);
}

void PlayWidget::updatewelcomename(QString userName)
{
    isLogined=true;
    loginBtn->setText("已登录");
    welcomeLabel->setText(QString("欢迎!%1").arg(userName));

}

void PlayWidget::showplaywidget()
{
    this->show();      // 显示主界面
    this->loginWidget->hide();     // 隐藏登录界面
}
void PlayWidget::onMonitorBtnClicked()
{
    currentWidgetId=0;
    stackedWidget->setCurrentWidget(monitorWidget);
}
void PlayWidget::onPlaybackBtnClicked()
{
    currentWidgetId=1;
    stackedWidget->setCurrentWidget(playBackWidget);
}
void PlayWidget::onLogBtnClicked()
{
    currentWidgetId=2;
    stackedWidget->setCurrentWidget(logWidget);
}
void PlayWidget::onSettingBtnClicked()
{
    currentWidgetId=3;
    stackedWidget->setCurrentWidget(sysSettingWidget);
}
void PlayWidget::onLoginBtnClicked()
{
    if(isLogined)
    {
        QMessageBox::information(this,"提示","您已登录");
        return;
    }
    this->loginWidget->show();
    this->hide();
}
 //响应setView的信号
void PlayWidget::onStartCamera()
{

    if(!monitorBtn->isEnabled())monitorBtn->setEnabled(true);
    if(!playbackBtn->isEnabled())playbackBtn->setEnabled(true);
    if(!logBtn->isEnabled())logBtn->setEnabled(true);

    QString videoPath=sysSettingWidget->getVideoPath();
    QString photoPath=sysSettingWidget->getPhotoPath();
    QString currentCamera=sysSettingWidget->getCurrentCamera();
    int collectTimes=sysSettingWidget->getCollectiontime();

    playBackWidget->setVideoDir(videoPath);
    playBackWidget->setphotoDir(photoPath);
    startCamera(currentCamera,videoPath,photoPath,collectTimes);

}
void PlayWidget::onNewVideoBuild(const QString& videoName,const QString& coverName,const int& framesCount)
{
    emit newVideoBuild(videoName,coverName,framesCount);
}
void PlayWidget::clearCacheFile()
{
    qDebug()<<sysSettingWidget->getVideoPath();
    QDir _videoDir(sysSettingWidget->getVideoPath()+"/");
    QDir _coverDir(sysSettingWidget->getVideoPath()+"/cover/");
    if (!_videoDir.exists()||!_coverDir.exists())
    {
        qDebug() << "目录不存在：" << _videoDir<<","<<_coverDir;
        return;
    }

    //设置过滤规则：只显示文件，不显示目录
    _videoDir.setFilter(QDir::Files | QDir::NoSymLinks); // 排除符号链接
    _coverDir.setFilter(QDir::Files | QDir::NoSymLinks); // 排除符号链接
    //设置文件名过滤
    _videoDir.setNameFilters(QStringList() << "*.cache" << "*.CACHE");
    _coverDir.setNameFilters(QStringList() << "*.cache" << "*.CACHE");
    //封面缓存
    QFileInfoList coverFileInfoList = _coverDir.entryInfoList();
    for (const QFileInfo& fileInfo : coverFileInfoList)
    {
        qDebug()<<"删除封面缓存";
       QFile file(fileInfo.absoluteFilePath());
       if(!file.remove())
       {
           qDebug()<<"删除封面缓存失败";
       }
    }
    //视频缓存
    QFileInfoList videoFileInfoList = _videoDir.entryInfoList();
    for (const QFileInfo& fileInfo : videoFileInfoList)
    {

        QFile file(fileInfo.absoluteFilePath());
        if(!file.remove())
        {
            qDebug()<<"删除缓存失败";
        }
    }
}
void PlayWidget::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);
    QApplication::exit(0);
}
