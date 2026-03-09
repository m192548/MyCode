#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCalendarWidget>
#include <QDialogButtonBox>
#include <QDate>
#include <QDialog>
#include "mycalendarwidget.h"
#include <QEvent>
#include "videoplaywidget.h"
#include <QResizeEvent>
#include <QPixmap>
#include <QListWidgetItem>
class PlayListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayListWidget(QWidget *parent = nullptr);
    ~PlayListWidget();
    void initConnect();
    void initUi();
    void initQSS();
    void initWin();
    void findVideo();
    void setVideoDir(const QString& videoDir);
    void setPhotoDir(const QString& photoDir);
    void showCurrentPageVideoNames();
    bool eventFilter(QObject *watched, QEvent *event)override;
protected:
    void resizeEvent(QResizeEvent* event)override;
signals:
    void currentVideoChanged();
    void newVideoBuild(const QString videoName,const QString& coverName,const int& framesCount);


public slots:
    void onFindBtnClicked();
    void onPrePageBtnClicked();
    void onNextPageBtn();
    void onCurrentVideoNameChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void onNewVideoBuild(const QString videoName,const QString& coverName,const int& framesCount);

private:
    QLabel* videoLabel;
    QWidget* rightWidget;
    QListWidget* videoListWidget;
    QPushButton* findBtn;
    QPushButton *prePageBtn,*nextPageBtn;
    QLabel* pageCountLabel;
    VideoPlayWidget* videoPlayWidget;

    QVBoxLayout* rightLayout;
    QHBoxLayout* paginationBtnLayout;
    QWidget* paginationBtnWidget;
    QHBoxLayout* totalLayout;

    //日历对话框
    QDialog calendarDialog;
    QVBoxLayout dialogLayout;
    MyCalendarWidget calendar;
    QDialogButtonBox btnBox;
    QPushButton okBtn,cancelBtn;

private:
    QDate startDate,endDate;//要显示的视频日期范围
    QList<QString> videoNames;//视频名称
    QList<QString> coverNames;//封面名称
    QList<int> videoTotalFrames;//视频总帧数
    QString videoPath;        //视频路径
    QString coverPath;        //封面路径
    int currentPageId;      //当前页
    int totalPage;          //总页数
    int singelPageNamecount;//每页显示的数量

    int currentVideoIdx;     //当前选中的视频下标
    QPixmap curCoverPix;     //当前视频封面
};

#endif // PLAYLISTWIDGET_H
