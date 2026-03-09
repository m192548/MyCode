#include "syssettingwidget.h"
#include <QApplication>
#include <QMessageBox>
#include <QDir>
SysSettingWidget::SysSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    collectionTimes=0;
    initwin();
    initUI();
    initconnect();
    loadconfig();
    initQss();
}

void SysSettingWidget::initUI()
{
    totalLayout=new QVBoxLayout(this);
    placeHolderTop1=new QLabel(this);//占位拉伸高度
    placeHolderTop1->setFixedHeight(50);
    placeHolderTop2=new QLabel(this);//占位拉伸高度
    placeHolderTop2->setFixedHeight(80);
    placeHolderBottom=new QLabel(this);
    placeHolderBottom->setFixedHeight(100);
    this->titlelab=new QLabel("设置界面",this);//标题设置
    this->titlelab->setAlignment(Qt::AlignCenter);
    this->titlelab->setStyleSheet("font-size:45px;color:white");
    this->titlelab->setFixedHeight(100);

    gridWidget=new QWidget(this);
    gridLayout=new QGridLayout(gridWidget);
    //视频储存位标签
    this->videostorelab=new QLabel("视频储存位",this);
    this->videostorelab->setFixedSize(150,40);
    //视频储存位编辑框
    this->videopathedit=new QLineEdit("",this);
    this->videopathedit->setFixedSize(200,40);
    this->videopathedit->setReadOnly(true);//设置为只读,手动无法输入
    //视频储存位按钮
    this->videostorebtn=new QPushButton("...",this);
    this->videostorebtn->setFixedSize(50,40);
    //图片储存位标签
    this->photostorelab=new QLabel("图片储存位",this);
    this->photostorelab->setFixedSize(150,40);
    //图片储存位编辑框
    this->photopathedit=new QLineEdit("",this);
    this->photopathedit->setFixedSize(200,40);
    this->photopathedit->setReadOnly(true);
    //图片储存位按钮
    this->photostorebtn=new QPushButton("...",this);
    this->photostorebtn->setFixedSize(50,40);
    //采集时间标签
    this->collectiontimelab=new QLabel("采集时间",this);
    this->collectiontimelab->setFixedSize(150,40);
    //采集时间下拉框
    this->collectiontimebox=new QComboBox(this);
    //添加采集时间选项
    this->collectiontimebox->addItem("30秒");
    this->collectiontimebox->addItem("1分钟");
    this->collectiontimebox->addItem("3分钟");
    this->collectiontimebox->addItem("5分钟");
    //设置采集时间下拉框位置
    this->collectiontimebox->setFixedSize(200,40);
    //摄像头配置标签
    this->camerasetlab=new QLabel("摄像头配置",this);
    this->camerasetlab->setFixedSize(150,40);
    //摄像头下拉框位置
    this->camerasebox=new QComboBox(this);
    this->camerasebox->setFixedSize(200,40);
    QList<QCameraInfo> cameras=QCameraInfo::availableCameras();  // 枚举所有可用摄像头并添加到下拉框
    //从cameras列表中选出每个camera的信息
    for(const QCameraInfo &camera:cameras){
        this->camerasebox->addItem(camera.description());// 添加摄像头名称
        cameraNames.push_back(camera.description());
        camerasebox->addItem(camera.description());
    }
    placeHoderLeft=new QLabel(gridWidget);
    placeHoderRight=new QLabel(gridWidget);


    gridLayout->addWidget(placeHoderLeft,0,0);//占位

    gridLayout->addWidget(videostorelab,0,1);
    gridLayout->addWidget(videopathedit,0,2);
    gridLayout->addWidget(videostorebtn,0,3);

    gridLayout->addWidget(placeHoderRight,0,4);//占位

    gridLayout->addWidget(photostorelab,1,1);
    gridLayout->addWidget(photopathedit,1,2);
    gridLayout->addWidget(photostorebtn,1,3);
    gridLayout->addWidget(collectiontimelab,2,1);
    gridLayout->addWidget(collectiontimebox,2,2);
    gridLayout->addWidget(camerasetlab,3,1);
    gridLayout->addWidget(camerasebox,3,2);

    gridLayout->setColumnStretch(0,4);
    gridLayout->setColumnStretch(1,1);
    gridLayout->setColumnStretch(2,1);
    gridLayout->setColumnStretch(3,1);
    gridLayout->setColumnStretch(4,4);

    gridLayout->setRowStretch(0,1);
    gridLayout->setRowStretch(1,1);
    gridLayout->setRowStretch(2,1);
    gridLayout->setRowStretch(3,1);



    hLayoutWidget=new QWidget(this);
    hLayout=new QHBoxLayout(hLayoutWidget);

    //确定按钮
    this->gologinbtn=new QPushButton("确定",this);
    this->gologinbtn->setFixedSize(100,40);
    //取消按钮
    this->returnbtn=new QPushButton("取消",this);
    this->returnbtn->setFixedSize(100,40);
    hLayout->addStretch();
    hLayout->addWidget(gologinbtn);
    hLayout->addStretch();
    hLayout->addWidget(returnbtn);
    hLayout->addStretch();


     totalLayout->addWidget(placeHolderTop1);
    totalLayout->addWidget(titlelab);
     totalLayout->addWidget(placeHolderTop2);
    totalLayout->addWidget(gridWidget);
    totalLayout->addWidget(hLayoutWidget);
    totalLayout->addWidget(placeHolderBottom);


}

void SysSettingWidget::initwin()
{
    this->setWindowTitle("设置界面");
    this->resize(900,600);
    this->setWindowIcon(QIcon(":/images/icon.png"));
}

void SysSettingWidget::initconnect()
{
    connect(videostorebtn,SIGNAL(clicked()),this,SLOT(videostorebtnclicked()));
    connect(photostorebtn,SIGNAL(clicked()),this,SLOT(photostorebtnclicked()));
    connect(this->gologinbtn,SIGNAL(clicked()),this,SLOT(saveconfig()));
    connect(this->returnbtn,SIGNAL(clicked()),this,SLOT(close()));
    connect(this->camerasebox,SIGNAL(currentIndexChanged(int)),this,SLOT(devicechange()));
}

void SysSettingWidget::paintEvent(QPaintEvent *event)
{
    //背景自适应
    QPainter paint(this);
    paint.drawPixmap(this->rect(),QPixmap(":/images/background1.png"));
}
void SysSettingWidget::initQss()
{
    //加载外部qss文件
    QFile file(":/qss/setview.qss");//资源文件路径
    if(file.open(QFile::ReadOnly))
    {
        QString styleSheet=QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

}

bool SysSettingWidget::checkconfig()
{
    if(videoPath.isEmpty())
    {
        QMessageBox::information(this,"提示","视频路径无效");
        return false;
    }
    if(photoPath.isEmpty())
    {
        QMessageBox::information(this,"提示","图片路径无效");
        return false;
    }
    if(currentCamera.isEmpty())
    {
        QMessageBox::information(this,"提示","摄像头配置无效");
        return false;
    }
    if(collectionTimes==0)
    {
        QMessageBox::information(this,"提示","采集时间无效");
        return false;
    }
    return true;

}

void SysSettingWidget::loadconfig()
{
    QString appPath=QCoreApplication::applicationDirPath();
    QSettings setting(appPath+"/config.ini",QSettings::IniFormat);//创建QSettings对象并指定文件ini文件路径并将格式设置为ini
    setting.beginGroup("video");//配置[video组]
    //加载视频配置
    this->videoPath=setting.value("video_path","").toString();
    //初始设置视频路径为程序所在目录
    if(this->videoPath.isEmpty())
    {
        this->videoPath=appPath+"/fileOut/video";
        setting.setValue("video_path",this->videoPath);
    }
    this->videopathedit->setText(videoPath);
    //加载摄像头配置
    this->currentCamera=setting.value("camera_name","Integrated Camera").toString();
    //从配置文件中读取 [video] 组下的 camera_name 键的值；如果该键不存在（比如首次运行程序），则使用默认值Integrated Camera
    int cameraIdx = camerasebox->findText(this->currentCamera);//查找下拉框中匹配的摄像头选项
    if(cameraIdx >= 0){
        camerasebox->setCurrentIndex(cameraIdx);//选中匹配的摄像头选项
    }
    else
    {
        if(!cameraNames.isEmpty())
        this->currentCamera=cameraNames[0];
        setting.setValue("camera_name",this->currentCamera);//保存摄像头设置

    }
    //加载采集时间配置
    this->collectionTimes=setting.value("collect_time",60).toInt();//toint进行类型转换
    if(collectionTimes==30){
        collectiontimebox->setCurrentText("30秒");
    }else if(collectionTimes==60){
        collectiontimebox->setCurrentText("1分钟");
    }else if(collectionTimes==180){
        collectiontimebox->setCurrentText("3分钟");
    }else if(collectionTimes==300){
        collectiontimebox->setCurrentText("5分钟");
    }
    setting.endGroup();
    setting.beginGroup("photo");//配置[photo组]
    //加载图片配置
    this->photoPath=setting.value("photo_path","").toString();
    //初始设置视频路径为程序所在目录
    if(this->photoPath.isEmpty())
    {
        this->photoPath=appPath+"/fileOut/photo";
        setting.setValue("photo_path",this->photoPath);
    }
    this->photopathedit->setText(this->photoPath);
    setting.endGroup();
    qDebug()<<"加载配置完成";
}

void SysSettingWidget::saveconfig()
{
    QString appPath=QCoreApplication::applicationDirPath();
    QSettings setting(appPath+"/config.ini",QSettings::IniFormat);//创建QSettings对象并指定文件ini文件路径并将格式设置为ini
    setting.beginGroup("video");//配置[video组]
    this->videoPath=videopathedit->text();
    //创建封面目录
    QDir videoDir(this->videoPath);
    //不存在cover目录则创建
    if(!videoDir.exists("cover"))
    {
        if(!videoDir.mkdir("cover"))
        {
            QMessageBox::information(this,"设置界面","保存失败，无法创建子目录!");
            setting.endGroup();
            return;
        }
    }
    setting.setValue("video_path",this->videoPath);//保存视频配置
    this->currentCamera=camerasebox->currentText();
    setting.setValue("camera_name",this->currentCamera);//保存摄像头设置
    QString timeText = collectiontimebox->currentText();//保存时间设置
    this->collectionTimes = 60;
    if(timeText=="30秒"){
        this->collectionTimes = 30;
    }else if(timeText=="1分钟"){
        this->collectionTimes = 60;
    }else if(timeText == "3分钟"){
        this->collectionTimes = 180;
    }
    else if(timeText == "5分钟"){
        this->collectionTimes = 300;
    }
    setting.setValue("collect_time", this->collectionTimes);
    setting.endGroup();
    setting.beginGroup("photo");//配置[photo组]
    this->photoPath=photopathedit->text();
    setting.setValue("photo_path", photopathedit->text());
    setting.endGroup();
    qDebug()<<"保存配置完成";

    QMessageBox::information(this,"设置界面","保存成功!");
    emit configcompleted();//发信号给playWidget
}

void SysSettingWidget::devicechange()
{
    QMessageBox::information(this,"设备界面","摄像头变更,请点击[确定]保存新配置!");
    gologinbtn->setFocus();
}

void SysSettingWidget::videostorebtnclicked()
{
    QString selectedDir=QFileDialog::getExistingDirectory(this,tr("选择视频对应路径"),QDir::homePath());//父窗口 标题对话框 设置初始路径
    if(!selectedDir.isEmpty()){
        this->videopathedit->setText(selectedDir);//如果路径不为空 视频路径编辑框更新为选择的视频路径
    }
}

void SysSettingWidget::photostorebtnclicked()
{
    QString selectedDir=QFileDialog::getExistingDirectory(this,tr("选择图片对应路径"),QDir::homePath());
    if(!selectedDir.isEmpty()){
        this->photopathedit->setText(selectedDir);
    }
}
QString SysSettingWidget::getPhotoPath()const
{
    return photopathedit->text();
}
QString SysSettingWidget::getVideoPath()const
{
    return videopathedit->text();
}
int SysSettingWidget::getCollectiontime()const
{
    return collectionTimes;
}
QList<QString>& SysSettingWidget::getCameraNames()
{
    return cameraNames;
}
QString SysSettingWidget::getCurrentCamera()const
{
    return currentCamera;
}
