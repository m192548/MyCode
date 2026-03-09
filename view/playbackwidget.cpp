#include "playbackwidget.h"
#include <QDebug>
PlayBackWidget::PlayBackWidget(QWidget *parent) : QWidget(parent)
{
    initUi();
    initQSS();
    initConnect();
    initWin();
}
PlayBackWidget::~PlayBackWidget()
{

}
void PlayBackWidget::initConnect()
{
    connect(playListBtn,SIGNAL(clicked()),this,SLOT(onPlayListBtnClicked()));
    connect(photoListBtn,SIGNAL(clicked()),this,SLOT(onPhotoListBtnClicked()));
    connect(this,SIGNAL(newVideoBuild(const QString&,const QString&,const int&)),playListWidget,SLOT(onNewVideoBuild(const QString&,const QString&,const int&)));
}
void PlayBackWidget::onNewVideoBuild(const QString videoName,const QString& coverName,const int& framesCount)
{
    emit newVideoBuild(videoName,coverName,framesCount);
}
void PlayBackWidget::initUi()
{
    qDebug()<<"开始设置playBackWidget的ui";
    leftWidget=new QWidget(this);
    leftWidget->setObjectName("leftWidget");
    leftWidget->resize(100,this->height());
    leftWidget->setFixedWidth(150);
    leftWidgetLayout=new QVBoxLayout(leftWidget);
    leftWidgetLayout->setSpacing(0);
    leftWidgetLayout->setMargin(0);

    playListBtn=new QPushButton("视频回放",leftWidget);
    playListBtn->setFixedHeight(30);
    playListBtn->setObjectName("playListBtn");
    playListBtn->setCheckable(true);
    playListBtn->setChecked(true);
    photoListBtn=new QPushButton("照片列表",leftWidget);
    photoListBtn->setFixedHeight(30);
    photoListBtn->setObjectName("photoListBtn");
    photoListBtn->setCheckable(true);
    leftWidgetLayout->addWidget(playListBtn);
    leftWidgetLayout->addWidget(photoListBtn);
    leftWidgetLayout->addStretch();

    totalLayout=new QHBoxLayout(this);
    totalLayout->setSpacing(0);
    totalLayout->setMargin(0);
    stackedWidget=new QStackedWidget(this);
    playListWidget=new PlayListWidget(stackedWidget);
    photoListWidget=new PhotoListWidget(stackedWidget);
    stackedWidget->addWidget(playListWidget);
    stackedWidget->addWidget(photoListWidget);
    stackedWidget->setCurrentWidget(playListWidget);

    totalLayout->addWidget(leftWidget);
    totalLayout->addWidget(stackedWidget);
    qDebug()<<"设置playBackWidget的ui结束";


}
void PlayBackWidget::initWin()
{

}
void PlayBackWidget::initQSS()
{
    //加载外部qss文件
    QFile file(":/qss/playbackwidget.qss");//资源文件路径
    if(file.open(QFile::ReadOnly))
    {
        QString styleSheet=QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

}


void PlayBackWidget::onPlayListBtnClicked()
{
stackedWidget->setCurrentWidget(playListWidget);
playListBtn->setChecked(true);
photoListBtn->setChecked(false);
}
void PlayBackWidget::onPhotoListBtnClicked()
{
stackedWidget->setCurrentWidget(photoListWidget);
photoListBtn->setChecked(true);
playListBtn->setChecked(false);
}
void PlayBackWidget::setVideoDir(const QString& videoDir)
{
    this->videoDir=videoDir;
    playListWidget->setVideoDir(this->videoDir);
}
void PlayBackWidget::setphotoDir(const QString& photoDir)
{
    this->photoDir=photoDir;
    photoListWidget->setphotoDir(this->photoDir);
    playListWidget->setPhotoDir(this->photoDir);
}
