#include "playlistwidget.h"
#include <QDialog>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QPixmap>
#include <QFile>
#include <QFileInfo>
#include "model/singletondb.h"
PlayListWidget::PlayListWidget(QWidget *parent) : QWidget(parent)
{
        currentPageId=0;
        currentVideoIdx=-1;
        totalPage=0;
        singelPageNamecount=10;
        videoPlayWidget=new VideoPlayWidget;
        initUi();
        initQSS();
        initConnect();
        initWin();
}
PlayListWidget::~PlayListWidget()
{
    if(videoPlayWidget)
        videoPlayWidget->deleteLater();
}
void PlayListWidget::initConnect()
{
    connect(findBtn,SIGNAL(clicked()),SLOT(onFindBtnClicked()));
    connect(prePageBtn,SIGNAL(clicked()),SLOT(onPrePageBtnClicked()));
    connect(nextPageBtn,SIGNAL(clicked()),SLOT(onNextPageBtn()));
    connect(&btnBox, SIGNAL(accepted()), &calendarDialog,SLOT(accept()));
    connect(&btnBox, SIGNAL(rejected()), &calendarDialog,SLOT(reject()));
    connect(videoListWidget,SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem*)),this,SLOT(onCurrentVideoNameChanged(QListWidgetItem *, QListWidgetItem*)));
    connect(this,SIGNAL(currentVideoChanged()),videoPlayWidget,SLOT(onCurrentVideoChanged()));
}
void PlayListWidget::initUi()
{
    calendarDialog.setWindowTitle("选择日期范围");
    calendarDialog.setWindowIcon(QIcon(":/images/icon.png"));
    calendarDialog.setFixedSize(400,250);
    calendar.setMinimumDate(QDate(2000, 1, 1));
    calendar.setMaximumDate(QDate::currentDate());
    dialogLayout.addWidget(&calendar);
    dialogLayout.addWidget(&btnBox);
    calendarDialog.setLayout(&dialogLayout);
    btnBox.setFixedSize(350,30);
    okBtn.setText("确定");
    okBtn.setFixedSize(80,25);
    cancelBtn.setText("取消");
    cancelBtn.setFixedSize(80,25);
    btnBox.addButton(&okBtn,QDialogButtonBox::AcceptRole);
    btnBox.addButton(&cancelBtn,QDialogButtonBox::RejectRole);

    //总布局
    totalLayout=new QHBoxLayout(this);
    //显示视频的label
    videoLabel=new QLabel("默认播放最后一个视频",this);
    videoLabel->setMinimumSize(600,500);
    qDebug()<<"videoLabel size:"<<videoLabel->size();
    videoLabel->setObjectName("videoLabel");
    videoLabel->setAlignment(Qt::AlignCenter);
    videoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    videoLabel->installEventFilter(this);
    //右侧视图:选择视频，显示视频列表，选择页面
    rightWidget= new QWidget(this);
    rightWidget->setFixedWidth(200);
    rightWidget->setObjectName("rightWidget");
    rightLayout=new QVBoxLayout(rightWidget);
    rightLayout->setMargin(0);
    findBtn=new QPushButton("查询",rightWidget);
    findBtn->setObjectName("findBtn");
    findBtn->setFixedSize(80,30);

    videoListWidget=new QListWidget(rightWidget);
    videoListWidget->setObjectName("videoListWidget");
    videoListWidget->resize(250,400);
    videoListWidget->setFixedWidth(250);

    //右侧底部视图:选择页面
    paginationBtnWidget=new QWidget(rightWidget);
    paginationBtnLayout=new QHBoxLayout(paginationBtnWidget);
    prePageBtn=new QPushButton("<",paginationBtnWidget);
    pageCountLabel=new QLabel("0/0",paginationBtnWidget);
    pageCountLabel->setObjectName("pageCountLabel");
    nextPageBtn=new QPushButton(">",paginationBtnWidget);
    paginationBtnLayout->addStretch();
    paginationBtnLayout->addWidget(prePageBtn);
    paginationBtnLayout->addWidget(nextPageBtn);
    paginationBtnLayout->addWidget(pageCountLabel);
    paginationBtnLayout->addStretch();

    rightLayout->addWidget(findBtn);
    rightLayout->addWidget(videoListWidget);
    rightLayout->addWidget(paginationBtnWidget);

    //把视频显示标签和右侧窗口放到总布局
    totalLayout->addWidget(videoLabel);
    totalLayout->addWidget(rightWidget);
    totalLayout->setStretch(0,9);
    totalLayout->setStretch(1,1);
    totalLayout->setSpacing(0);
    totalLayout->setMargin(0);

}
void PlayListWidget::initQSS()
{
    //加载外部qss文件
    QFile file(":/qss/playlistwidget.qss");//资源文件路径
    if(file.open(QFile::ReadOnly))
    {
        QString styleSheet=QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }
}
void PlayListWidget::initWin()
{

}
bool PlayListWidget::eventFilter(QObject *watched, QEvent *event)
{
    //检测鼠标点击事件
    if(event->type()==QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent=static_cast<QMouseEvent*>(event);
        if(mouseEvent->button()==Qt::LeftButton)
        {
            //判断鼠标是否是视频标签
            if(watched==videoLabel)
            {
                if(videoPlayWidget->isVisible())
                {
                    qDebug()<<"窗口已经打开";
                    return QWidget::eventFilter(watched, event);
                }
                if(currentVideoIdx==-1)return QWidget::eventFilter(watched, event);
                videoPlayWidget->show();
            }
        }
    }
    return QWidget::eventFilter(watched, event); // 其他事件交给父类处理
}

void PlayListWidget::onFindBtnClicked()
{
    if(QDialog::Accepted==calendarDialog.exec())
    {
        startDate=calendar.getStartDate();
        endDate=calendar.getEndDate();
        findVideo();
        showCurrentPageVideoNames();
        if(videoNames.empty())return;
        curCoverPix=QPixmap(videoPath+"/cover/"+coverNames[currentVideoIdx]);
        videoLabel->setPixmap(curCoverPix.scaled(videoLabel->size()));
        int row=videoNames.size()%singelPageNamecount==0?singelPageNamecount-1:videoNames.size()%singelPageNamecount-1;
        videoListWidget->item(row)->setSelected(true);

    }
    else
    {
        calendar.setStartDate(startDate);
        calendar.setEndDate(endDate);
        calendarDialog.hide();
    }


}
void PlayListWidget::onPrePageBtnClicked()
{
    if(currentPageId==1||currentPageId==0)return;
    currentPageId--;
    showCurrentPageVideoNames();
    //如果是当前视频所在页
     qDebug()<<"当前页:"<<currentPageId<<",当前video下标:"<<currentVideoIdx;
    if(currentVideoIdx>=((currentPageId-1)*singelPageNamecount)&&
            currentVideoIdx<=(currentPageId)*singelPageNamecount-1)
    {
        qDebug()<<"是当前视频所在页:";
        int row=currentVideoIdx%singelPageNamecount;
        videoListWidget->item(row)->setSelected(true);
    }


}
void PlayListWidget::onNextPageBtn()
{
    if(currentPageId>=totalPage||currentPageId==0)return;
    currentPageId++;
    showCurrentPageVideoNames();
     qDebug()<<"当前页:"<<currentPageId<<",当前video下标:"<<currentVideoIdx;
    if(currentVideoIdx>=((currentPageId-1)*singelPageNamecount)&&
            currentVideoIdx<=(currentPageId)*singelPageNamecount-1)
    {
         qDebug()<<"是当前视频所在页:";
        int row=currentVideoIdx%singelPageNamecount;
        videoListWidget->item(row)->setSelected(true);
    }
}


void PlayListWidget::findVideo()
{
    videoNames.clear();
    coverNames.clear();
    videoTotalFrames.clear();
    currentPageId=0;
    currentVideoIdx=-1;
    totalPage=0;

    QDir _videoDir(videoPath+"/");
    QDir _coverDir(videoPath+"/cover/");
    //设置过滤规则：只显示文件，不显示目录
    _videoDir.setFilter(QDir::Files | QDir::NoSymLinks); // 排除符号链接
    _coverDir.setFilter(QDir::Files | QDir::NoSymLinks); // 排除符号链接
    //设置文件名过滤：仅匹配.h264后缀
    _videoDir.setNameFilters(QStringList() << "*.h264" << "*.H264");
    _coverDir.setNameFilters(QStringList() << "*.png" << "*.PNG");
    //获取封面文件列表
    QFileInfoList coverFileInfoList = _coverDir.entryInfoList();
    for (const QFileInfo& fileInfo : coverFileInfoList)
    {
        if(startDate.isValid()&&endDate.isValid())
        {
            if(fileInfo.created().date()>=startDate&&fileInfo.created().date()<=endDate)
            {
                 coverNames.append(fileInfo.fileName());
            }
        }
        else if(!startDate.isValid()&&!endDate.isValid())
        {
             coverNames.append(fileInfo.fileName());
        }
        else if(startDate.isValid())
        {
            if(fileInfo.created().date()==startDate)
            {
                coverNames.append(fileInfo.fileName());
            }
        }

    }
    if(coverNames.empty())
    {
        videoLabel->clear();
        videoLabel->setText("暂无视频");
        return;
    }
    //获取视频文件列表并遍历
    QFileInfoList videoFileInfoList = _videoDir.entryInfoList();
    //根据封面查找对应视频文件
    bool isHave=false;
    for (const QFileInfo& cover_fileInfo : coverFileInfoList)
    {
        int pos1=cover_fileInfo.fileName().lastIndexOf('.');
        QString cover_name=cover_fileInfo.fileName().left(pos1);
        for (const QFileInfo& video_fileInfo : videoFileInfoList)
        {
            int pos2 =video_fileInfo.fileName().lastIndexOf('.');
            QString video_name=video_fileInfo.fileName().left(pos2);
            if(video_name==cover_name)//找到该封面对应的视频
            {
                 videoNames.append(video_fileInfo.fileName());
                 isHave=true;

                 //查找当前视频的总帧数
                 char** res=nullptr;
                 int row=0,col=0;
                  int totalFrames=0;
                 QString sql=QString("select video_total_frame from tbl_video where video_name = \"%1\"")
                        .arg(video_fileInfo.fileName());
                 if(SingletonDB::getInstance()->selectsql(sql,res,row,col)==0)
                 {
                     if(row==0)
                     {
                         qDebug()<<"无法获取视频:"<<video_fileInfo.fileName()<<"的帧数";
                     }
                     if(row==1)
                     {
                       totalFrames=QString(res[1]).toInt();
                     }
                     videoTotalFrames.push_back(totalFrames);

                 }
                break;
            }
        }
        if(isHave==false)
        {
            QFile file(cover_fileInfo.path()+"/"+cover_fileInfo.fileName());
            file.remove();
            file.close();
        }
        isHave=false;
    }
    //清理无封面的视频
    isHave=false;
    for (const QFileInfo& video_fileInfo : videoFileInfoList)
    {
        int pos1=video_fileInfo.fileName().lastIndexOf('.');
        QString video_name=video_fileInfo.fileName().left(pos1);
        for (const QFileInfo& cover_fileInfo : coverFileInfoList)
        {
            int pos2=cover_fileInfo.fileName().lastIndexOf('.');
            QString cover_name=cover_fileInfo.fileName().left(pos2);
            if(cover_name==video_name)
            {
                isHave=true;
                break;
            }
        }
        if(isHave==false)
        {
            QFile file(video_fileInfo.path()+"/"+video_fileInfo.fileName());
            file.remove();
            file.close();
        }
        isHave=false;
    }
    if(!coverNames.empty()&&!videoNames.empty()&&!videoTotalFrames.empty())
    {
        totalPage=videoNames.size()/10+(videoNames.size()%10==0?0:1);
        if(totalPage>0)
        {
            //默认是最后一个视频
            currentVideoIdx=videoNames.size()-1;
            currentPageId=totalPage;
        }
        else
        {
            currentPageId=0;
            currentVideoIdx=-1;
        }
        QString videoFilePath=videoPath+"/"+videoNames[currentVideoIdx];
        QString coverFilePath=coverPath+"/"+coverNames[currentVideoIdx];
        //设置播放界面的视频路径，封面路径和帧数
        videoPlayWidget->setVideoFilePath(videoFilePath);
        videoPlayWidget->setCoverFilePath(coverFilePath);
        videoPlayWidget->setVideoFrames(videoTotalFrames[currentVideoIdx]);

        //发信号给播放界面
        emit currentVideoChanged();
        //加载当前视频封面
        curCoverPix.load(coverFilePath);
    }



}
void PlayListWidget::setVideoDir(const QString& videoDir)
{
    this->videoPath=videoDir;
    this->coverPath=this->videoPath+"/cover";
    findVideo();
    showCurrentPageVideoNames();
    if(videoNames.empty())return;
    int row=videoNames.size()%singelPageNamecount==0?singelPageNamecount-1:videoNames.size()%singelPageNamecount-1;
    videoListWidget->item(row)->setSelected(true);
}
void PlayListWidget::setPhotoDir(const QString& photoDir)
{
    videoPlayWidget->setPhotoPath(photoDir);
}
void PlayListWidget::showCurrentPageVideoNames()
{
    videoListWidget->clear();
    if(videoNames.empty())
    {
        QString text=QString("0/0");
        pageCountLabel->setText(text);
       videoLabel->clear();
       videoLabel->setText("暂无视频");
        return;
    }

    //每页显示10条
    for(int i=0;i<singelPageNamecount;i++)
    {

        int index=(currentPageId-1)*singelPageNamecount+i;
        if(index>=videoNames.size())break;
        videoListWidget->addItem(videoNames[index]);
    }
    QString text=QString("%1/%2").arg(currentPageId).arg(totalPage);
    pageCountLabel->setText(text);

}
void PlayListWidget::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
     if(!curCoverPix.isNull())
     {
         videoLabel->setPixmap(curCoverPix.scaled(videoLabel->size()));
     }
}
void PlayListWidget::onCurrentVideoNameChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(!current||currentPageId==0)return;
    currentVideoIdx=(currentPageId-1)*singelPageNamecount+videoListWidget->row(current);
    if(currentVideoIdx>=0)
    {
        curCoverPix=QPixmap(videoPath+"/cover/"+coverNames[currentVideoIdx]);
        videoLabel->setPixmap(curCoverPix.scaled(videoLabel->size()));
        //播放窗口
        //设置视频文件路径
        QString videoFilePath=videoPath+"/"+videoNames[currentVideoIdx];
        videoPlayWidget->setVideoFilePath(videoFilePath);
        //设置视频封面路径
        QString coverFilePath=coverPath+"/"+coverNames[currentVideoIdx];
        videoPlayWidget->setCoverFilePath(coverFilePath);
        //设置视频总帧数
        videoPlayWidget->setVideoFrames(videoTotalFrames[currentVideoIdx]);
        //发信号给播放界面
        emit currentVideoChanged();
    }

}
void PlayListWidget::onNewVideoBuild(const QString videoName,const QString& coverName,const int& framesCount)
{
    QFileInfo fileInfo(videoPath+"/"+videoName);
    if((videoNames.size()%singelPageNamecount==0)&&currentPageId==totalPage)
    {
        totalPage++;
        if(currentPageId==0)
        {
            currentPageId++;
           //判断当前视频文件是否在选中日期内
            if((startDate.isValid()&&endDate.isValid()))
            {
                if(fileInfo.created().date()>=startDate&&
                  fileInfo.created().date()<=endDate)
                {
                    videoListWidget->addItem(videoName);
                   videoListWidget->item(0)->setSelected(true);
                }
            }
            else if((startDate.isValid()&&!endDate.isValid()))
            {
                   if(fileInfo.created().date()==startDate)
                   {
                       videoListWidget->addItem(videoName);
                      videoListWidget->item(0)->setSelected(true);
                   }
            }
            else
            {
                videoListWidget->addItem(videoName);
               videoListWidget->item(0)->setSelected(true);
            }

        }
        QString text=QString("%1/%2").arg(currentPageId).arg(totalPage);
        pageCountLabel->setText(text);
    }
    else if(currentPageId==totalPage)
    {
        //判断当前视频文件是否在选中日期内
        if((startDate.isValid()&&endDate.isValid()))
        {
            if(fileInfo.created().date()>=startDate&&
              fileInfo.created().date()<=endDate)
            {
                videoListWidget->addItem(videoName);
            }
        }
        else if((startDate.isValid()&&!endDate.isValid()))
        {
               if(fileInfo.created().date()==startDate)
               {
                   videoListWidget->addItem(videoName);
               }
        }
        else
        {
            videoListWidget->addItem(videoName);
        }
    }
   videoNames.push_back(videoName);
   coverNames.push_back(coverName);
   videoTotalFrames.push_back(framesCount);
}
