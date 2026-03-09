#ifndef PLAYBACKWIDGET_H
#define PLAYBACKWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include "playlistwidget.h"
#include "photolistwidget.h"
class PlayBackWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayBackWidget(QWidget *parent = nullptr);
    ~PlayBackWidget();
    void initConnect();
    void initUi();
    void initWin();
    void initQSS();
    void setVideoDir(const QString& videoDir);
    void setphotoDir(const QString& photoDir);
signals:
    void newVideoBuild(const QString videoName,const QString& coverName,const int& framesCount);
public slots:
    void onPlayListBtnClicked();
    void onPhotoListBtnClicked();
    void onNewVideoBuild(const QString videoName,const QString& coverName,const int& framesCount);

private:
    QWidget* leftWidget;
    QVBoxLayout* leftWidgetLayout;
    QPushButton* playListBtn,*photoListBtn;
    QStackedWidget* stackedWidget;

    QHBoxLayout* totalLayout;
    PlayListWidget* playListWidget;
    PhotoListWidget* photoListWidget;

private:
    QString videoDir;
    QString photoDir;
};

#endif // PLAYBACKWIDGET_H
