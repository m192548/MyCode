#ifndef MYITEM_H
#define MYITEM_H
#include <QObject>
#include <QGraphicsItem>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QStyleOptionGraphicsItem>
#include <QString>
class MyItem : public QObject,public QGraphicsItem
{
    Q_OBJECT
public:
    enum Direction{left,right};//定义方向枚举
    explicit MyItem(QString path,bool status,Direction dir);//图片路径 图片状态 --true 移动 --false不移动
//移动方向
    //重写方法 返回图元所在位置
    QRectF boundingRect() const;
    //重写方法 重绘图元
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget=Q_NULLPTR);
    void advance(int phase);
signals:
    void boom();
public slots:
private:
    QPixmap img;
    bool status;
    Direction dir;
};

#endif // MYITEM_H
