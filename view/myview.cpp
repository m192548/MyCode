#include "myview.h"
#include <QPixmap>
#include <QBrush>
#include <QIcon>
#include <QDebug>

MyView::MyView()
{
    this->play=nullptr;
     this->play=new PlayWidget();
    mytimer = new QTimer(this);
    //1.设置窗口大小和背景
    this->setFixedSize(1042,700);
    this->setWindowTitle("开机动画");//标题
    this->setWindowIcon(QIcon(":/images/icon.png"));//开机动画图标
    this->setBackgroundBrush(QBrush(QPixmap(":/images/bg.png")));//开机动画背景
    this->label=new QLabel("欢迎",this);//欢迎标题
    this->label->setStyleSheet("font-size:50px;color:white");//欢迎标题样式设置
    this->label->setGeometry(450,100,100,100);//欢迎标题位置设置
    this->scene=new QGraphicsScene();//创建场景
    this->setScene(this->scene);//将场景添加到该视图
    this->setSceneRect(0,0,this->width()-2,this->height()-2);//重新设置坐标使坐标系统一
    this->item1=new MyItem(":/images/monitor1.png",true,MyItem::right);//创建图元
    this->item2=new MyItem(":/images/monitor1.png",true,MyItem::left);
    //添加到场景中
    this->scene->addItem(this->item1);
    this->scene->addItem(this->item2);
    //设置站位
    this->item1->setPos(150,this->height()/2);
    this->item2->setPos(850,this->height()/2);
    //创建定时器
    mytimer=new QTimer(this);
    mytimer->start(40);
    //每隔40ms,场景调用advance槽函数,调用自己管辖范围内的所有item的advance函数
    connect(mytimer,SIGNAL(timeout()),this->scene,SLOT(advance()));
    connect(this->item1,SIGNAL(boom()),this,SLOT(stopTimer()));
    connect(this->item2,SIGNAL(boom()),this,SLOT(oncollide()));
    connect(this,SIGNAL(startCamera()),this->play,SLOT(onStartCamera()));
}

void MyView::stopTimer()
{
    this->mytimer->stop();
    qDebug()<<"停止定时器";
}
void MyView::oncollide()
{
    this->mytimer->stop();
    this->item1->hide();
    this->item2->hide();
    this->hide();
   if(play->checkConfiguration())
   {
       play->setCurrentWidget(0);
       emit startCamera();
   }
   else
   {
       play->setCurrentWidget(3);
   }
   play->show();

}




