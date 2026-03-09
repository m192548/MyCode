#include "myitem.h"
#include <QDebug>
MyItem::MyItem(QString path,bool status,Direction dir)
{
    this->img.load(path);
    this->status=status;
    this->dir=dir;
}

QRectF MyItem::boundingRect() const
{
    return QRectF(-this->img.width()/2,-this->img.height()/2,this->img.width(),this->img.height());
}

void MyItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(-this->img.width()/2,-this->img.height()/2,this->img.width(),this->img.height(),this->img);
}

void MyItem::advance(int phase)
{
    if(this->status){
        if(this->collidingItems().size()>0){
            qDebug()<<"boom";
            this->status=false;
            emit boom();
            return;
        }
        if(dir==right){
            this->setPos(mapToScene(5,0)); //水平方向移动的距离 矢量 正右负左
                                           //垂直方向移动的距离 矢量 正右负左
        }else if(dir==left){
            this->setPos(mapToScene(-5,0));
        }

    }
}
