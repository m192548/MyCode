#include "monitorwidget.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QDateTime>
#include "model/singletondb.h"
MonitorWidget::MonitorWidget(QWidget *parent)
    : QWidget(parent)
{
    currentChannel=1;
    cameraIsOpened=false;
    cameraThread[0]=new CameraThread;
    cameraThread[1]=nullptr;
    cameraThread[2]=nullptr;
    cameraThread[3]=nullptr;
    initUi();
    initQSS();
    initConnect();
    initWin();

}
void MonitorWidget::initUi()
{
     //初始化左侧窗口
     this->leftWidgt = new QWidget(this);
     this->leftWidgt->resize(200,this->height());
     this->leftWidgt->setFixedWidth(200);
     this->leftWidgt->setObjectName("leftWidget");
     this->cameraListWidget=new QListWidget(this->leftWidgt);
     this->cameraListWidget->setObjectName("cameraListWidget");
     this->cameraListWidget->resize(200,this->height());
     this->cameraListWidget->setFixedWidth(200);
     this->cameraLabel=new QLabel("设备列表",this->leftWidgt);
     this->cameraLabel->setAlignment(Qt::AlignCenter);
     this->cameraLabel->setObjectName("cameraLab");
     this->cameraLabel->setFixedSize(200,25);
     this->cameraLabel->setFixedWidth(200);
     this->leftVLayout=new QVBoxLayout(this->leftWidgt);
     this->leftVLayout->addWidget(cameraLabel);
     this->leftVLayout->addWidget(cameraListWidget);
     leftVLayout->setSpacing(0);
     leftVLayout->setMargin(0);

     //初始化视频显示窗口
     this->VideoWidget = new QWidget(this);
    this->VideoWidget->resize(850,650);
    this->VideoWidget->setObjectName("videoWidget");
    this->VideoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
     this->videoGridLayout=new QGridLayout(VideoWidget);
     // 第1个标签：第0行第0列
     this->videolabel1=new QLabel(VideoWidget);//将标签放入容器
     this->videolabel1->setText("视频通道1");
     this->videolabel1->setObjectName("videoLab1");
     this->videolabel1->setAlignment(Qt::AlignCenter);//文字居中显示
     this->videolabel1->installEventFilter(this);//安装事件过滤器
     this->videolabel1->setMinimumSize(400, 300);
     this->videolabel1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
     // 第2个标签：第0行第1列
      this->videolabel2=new QLabel(VideoWidget);
      this->videolabel2->setText("视频通道2");
      this->videolabel2->setObjectName("videoLab2");
      this->videolabel2->setAlignment(Qt::AlignCenter);//文字居中显示
      this->videolabel2->setMinimumSize(400, 300);
      this->videolabel2->installEventFilter(this);
      this->videolabel2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
     // 第3个标签：第1行第0列
      this->videolabel3=new QLabel(VideoWidget);
      this->videolabel3->setText("视频通道3");
      this->videolabel3->setObjectName("videoLab3");
      this->videolabel3->setAlignment(Qt::AlignCenter);//文字居中显示
      this->videolabel3->setMinimumSize(400, 300);
      this->videolabel3->installEventFilter(this);
      this->videolabel3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

      //第4个标签：第1行第1列
      this->videolabel4=new QLabel(VideoWidget);
      this->videolabel4->setText("视频通道4");
      this->videolabel4->setObjectName("videoLab4");
      this->videolabel4->setAlignment(Qt::AlignCenter);//文字居中显示
      this->videolabel4->setMinimumSize(400, 300);
      this->videolabel4->installEventFilter(this);
      this->videolabel4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      //添加网格布局
      this->videoGridLayout->addWidget(videolabel1, 0, 0);
      this->videoGridLayout->addWidget(videolabel2, 0, 1);
      this->videoGridLayout->addWidget(videolabel3, 1, 0);
      this->videoGridLayout->addWidget(videolabel4, 1, 1);

      //设置网格布局行列拉伸因子
      this->videoGridLayout->setRowStretch(0, 1);
      this->videoGridLayout->setRowStretch(1, 1);
      this->videoGridLayout->setColumnStretch(0, 1);
      this->videoGridLayout->setColumnStretch(1, 1);
      this->videoGridLayout->setSpacing(30);//网格间距
      this->videoGridLayout->setContentsMargins(10,10,10,10);


    //初始化底部窗口
     this->bottomWidget =new QWidget(this);
     this->bottomHLayout=new QHBoxLayout(bottomWidget);
     this->singelChannelBtn=new QPushButton(bottomWidget);
     this->singelChannelBtn->setStyleSheet("border:none;");
     this->singelChannelBtn->setIcon(QIcon(":/images/singleway.png"));
     this->singelChannelBtn->setIconSize(QSize(40,40));
     this->fourChannelBtn=new QPushButton(bottomWidget);
     this->fourChannelBtn->setStyleSheet("border:none;");
     this->fourChannelBtn->setIcon(QIcon(":/images/wayfour.png"));
     this->fourChannelBtn->setIconSize(QSize(40,40));
     this->catchBtn=new QPushButton("拍照",this->bottomWidget);

     //将按钮添加到布局
     this->bottomHLayout->addWidget(singelChannelBtn);
     this->bottomHLayout->addWidget(fourChannelBtn);
     this->bottomHLayout->addWidget(catchBtn);
     this->bottomHLayout->addStretch();//添加弹簧

    //布局
    this->containerHLayout= new QHBoxLayout(this);
    this->vLayout=new QVBoxLayout;

    //把三个窗口添加到布局
    vLayout->addWidget(VideoWidget,1);
    vLayout->addWidget(bottomWidget);

    containerHLayout->addWidget(leftWidgt);
    containerHLayout->addLayout(vLayout);
    containerHLayout->setSpacing(0);
    containerHLayout->setMargin(0);


}
void MonitorWidget::initConnect()
{
    connect(fourChannelBtn,SIGNAL(clicked()),this,SLOT(onFourChannelBtnClicked()));
    connect(singelChannelBtn,SIGNAL(clicked()),this,SLOT(onSingelChannelBtnClicked()));
    connect(catchBtn,SIGNAL(clicked()),this,SLOT(onCatchBtnClicked()));
    connect(cameraThread[0], SIGNAL(frameReady(const QImage&,bool,const QString&)), this, SLOT(onImageReady(const QImage&,bool,const QString&)), Qt::QueuedConnection);
    connect(cameraThread[0], SIGNAL(errorOccurred(const QString&)), this, SLOT(onErrorOccurred(const QString&)));
    connect(cameraThread[0],SIGNAL(newVideoBuild(const QString&,const QString&,const int&)),this,SLOT(onNewVideoBuild(const QString&,const QString&,const int&)));
}
void MonitorWidget::initWin()
{
}
void MonitorWidget::initQSS()
{
    //加载外部qss文件
    QFile file(":/qss/monitorwidget.qss");//资源文件路径
    if(file.open(QFile::ReadOnly))
    {
        QString styleSheet=QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

}
bool MonitorWidget::eventFilter(QObject *watched, QEvent *event)
{
    //检测鼠标点击事件
    if(event->type()==QEvent::MouseButtonPress){
        QMouseEvent *mouseEvent=static_cast<QMouseEvent*>(event);
        if(mouseEvent->button()==Qt::LeftButton){
            //判断鼠标点击的那个是视频标签
            if(watched==videolabel1){
                currentChannel=1;
                showSingleChannel();
                return true;
            }
            else if(watched==videolabel2){
                currentChannel=2;
                showSingleChannel();
                return true;
            }
            else if(watched==videolabel3){
                currentChannel=3;
                showSingleChannel();
                return true;
            }
            else if(watched==videolabel4){
                currentChannel=4;
                showSingleChannel();
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event); // 其他事件交给父类处理
}
void MonitorWidget::setVideoPath(const QString& path)
{
    videoPath=path;
}
void MonitorWidget::setPhotoPath(const QString& path)
{
    photoPath=path;
}

void MonitorWidget::showSingleChannel()
{
    isSingleChannel=true;//切换为单通道模式
    // 隐藏其他通道
    this->videolabel1->setVisible(false);
    this->videolabel2->setVisible(false);
    this->videolabel3->setVisible(false);
    this->videolabel4->setVisible(false);
    // 根据通道号显示对应标签，并占据整个网格
    QLabel* curVideoLabel=videolabel1;
    switch (currentChannel) {
    case 1:
        curVideoLabel=videolabel1;
        break;
    case 2:
        curVideoLabel=videolabel2;
        break;
    case 3:
        curVideoLabel=videolabel3;
        break;
    case 4:
        curVideoLabel=videolabel4;
        break;
    default:
       return;
    }
    //让当前标签占满2x2网格的全部区域（行跨度2，列跨度2）
    curVideoLabel->setVisible(true);
    this->videoGridLayout->removeWidget(curVideoLabel);
    //覆盖整个网格
    this->videoGridLayout->addWidget(curVideoLabel, 0, 0, 2, 2);
    // 允许标签随容器扩展
    curVideoLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);


}

void MonitorWidget::onFourChannelBtnClicked()
{
    // 恢复所有通道显示
    this->videolabel1->show();
    this->videolabel2->show();
    this->videolabel3->show();
    this->videolabel4->show();
    // 重置网格布局的行列拉伸因子（均等分配）
    this->videoGridLayout->setRowStretch(0, 1);
    this->videoGridLayout->setRowStretch(1, 1);
    this->videoGridLayout->setColumnStretch(0, 1);
    this->videoGridLayout->setColumnStretch(1, 1);
    // 重置所有标签的最小尺寸（确保一致）
    videolabel1->setMinimumSize(400, 300);
    videolabel2->setMinimumSize(400, 300);
    videolabel3->setMinimumSize(400, 300);
    videolabel4->setMinimumSize(400, 300);
    // 强制布局重新计算
    videoGridLayout->update();
    this->VideoWidget->updateGeometry();
    //重新添加所有控件，恢复1行1列的跨度
    // 按2x2网格添加四个视频标签
    videoGridLayout->addWidget(videolabel1, 0, 0); // 第0行第0列
    videoGridLayout->addWidget(videolabel2, 0, 1); // 第0行第1列
    videoGridLayout->addWidget(videolabel3, 1, 0); // 第1行第0列
    videoGridLayout->addWidget(videolabel4, 1, 1); // 第1行第1列
    // 标记当前为四通道模式
    isSingleChannel = false;
}

void MonitorWidget::onSingelChannelBtnClicked()
{
    showSingleChannel();
}

void MonitorWidget::onCatchBtnClicked()
{
    if(!isSingleChannel)
    {
        QMessageBox::information(this,"提示","请在单通道下进行该操作");
        return;
    }
    QPixmap pix=this->videolabel1->pixmap()->copy();
    if(pix.isNull())
    {
         QMessageBox::information(this,"提示","图片为空");
         return;
    }
    QDateTime currentTime=QDateTime::currentDateTime();
    QString datetimeStr=currentTime.toString("yyMMddHmmss");
    QString photoName=datetimeStr+".jpg";
    QString path=photoPath+"/";
    QString filePathName=path+"/"+photoName;
    QPixmap scalePix=pix.scaled(1920,1080);
    if(scalePix.save(filePathName))
    {
        QString sql=QString("insert into tbl_photo"
                            "(photo_name,photo_path,photo_time)"
                            "values(\"%1\",\"%2\",\"%3\")")
                .arg(photoName).arg(path).arg(currentTime.toString("yyyy-MM-dd HH:mm:ss"));
        if(!SingletonDB::getInstance()->execsql(sql)==0)
        {
            qDebug()<<"写入数据库失败";
        }
        QMessageBox::information(this,"保存","保存成功");
    }
    else
    {
        QMessageBox::information(this,"保存","保存失败");
    }

}
void MonitorWidget::setCameraNames(QList<QString>& cameranames)
{
    if(cameranames.isEmpty())return;
    cameraNames.append(cameranames);
    for(int i=0;i<cameraNames.size();i++)
    {
          cameraListWidget->addItem(cameraNames[i]);
    }
}
void MonitorWidget::setCollectionTimes(const int& times)
{
    collectionTimes=times;
}
//响应playWidget的信号
void MonitorWidget::onStartCamera(const QString& cameraName,const QString& videoPath,const QString& photoPath,const int& videoCollectTimes)
{
    this->videoPath=videoPath;
    this->photoPath=photoPath;
    this->collectionTimes=videoCollectTimes;
    cameraThread[0]->setVideoPath(videoPath);
    cameraThread[0]->setCollectTimes(videoCollectTimes);
    if(cameraThread[0]->isRunning())
    {
        cameraThread[0]->quit();
        cameraIsOpened=false;
    }
    bool success = cameraThread[0]->openCamera("video="+cameraName);
    if (success) cameraThread[0]->startThread(); // 启动线程


    cameraIsOpened=true;

}
void MonitorWidget::onStopCamera()
{
    cameraThread[0]->stopCamera();
    cameraIsOpened=false;
    videolabel1->clear(); // 清空画面

}
void MonitorWidget::onImageReady(const QImage& img,bool isCover,const QString& coverFilename)
{

       //将QImage转为QPixmap
       QPixmap pixmap = QPixmap::fromImage(img);
       if(!pixmap)return;
       QPixmap scaledPixmap = pixmap.scaled(videolabel1->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
       videolabel1->setPixmap(scaledPixmap);
       if(isCover)
       {
           QString coverFilePath=videoPath+"/cover/"+coverFilename;
           pixmap.save(coverFilePath,"PNG");
       }

}
void MonitorWidget::onErrorOccurred(const QString& errMsg)
{
    qDebug() << "Camera Error:" << errMsg;
    QMessageBox::warning(this, "Camera Error", errMsg);
    onStopCamera(); // 出错时自动停止

}
void MonitorWidget::onNewVideoBuild(const QString& videoName,const QString& coverName,const int& framesCount)
{
    emit newVideoBuild(videoName,coverName,framesCount);
}


