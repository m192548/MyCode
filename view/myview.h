#ifndef MYVIEW_H
#define MYVIEW_H
#include <QGraphicsScene> //视图中成员 场景
#include <QGraphicsView> //视图
#include <QPaintEvent>
#include <QObject>
#include "myitem.h"
#include <QTimer>
#include <QLabel>
#include "view/playwidget.h"
class MyView:public QGraphicsView
{
    Q_OBJECT
public:
    MyView();
private:
    QGraphicsScene *scene;
    MyItem *item1,*item2;
    QTimer *mytimer;
    QLabel *label;
    PlayWidget *play;
signals:
    void startCamera();
public slots:
    void stopTimer();
    void oncollide();
};

#endif // MYVIEW_H
