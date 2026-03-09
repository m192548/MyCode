#ifndef SYSSETTINGWIDGET_H_
#define SYSSETTINGWIDGET_H_
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QPaintEvent>
#include <QFileDialog>
#include <QCamera> //摄像头类
#include <QCameraInfo>//获取摄像头信息
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QList>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
class SysSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SysSettingWidget(QWidget *parent = nullptr);
    void initUI();//静态控件
    void initwin();//设置窗口
    void initconnect();//信号和槽关联
    void paintEvent(QPaintEvent *event);
    void initQss();
    bool checkconfig();// 检查是否为初始配置
    QString getPhotoPath()const;
    QString getVideoPath()const;
    QString getCurrentCamera()const;
    int getCollectiontime()const;
    QList<QString>& getCameraNames();
signals:
    void configcompleted();
public slots:
    void videostorebtnclicked();//点击对应videopath按钮
    void photostorebtnclicked();
    void loadconfig();
    void saveconfig();
    void devicechange();//设备改变重置配置

private:
    QWidget* gridWidget;
    QWidget* hLayoutWidget;

    QVBoxLayout* totalLayout;
    QGridLayout* gridLayout;
    QHBoxLayout* hLayout;

    QLabel *titlelab,*videostorelab,*photostorelab,*collectiontimelab,*camerasetlab;
    QLabel *placeHolderTop1,*placeHolderTop2,*placeHoderLeft,*placeHoderRight,*placeHolderBottom;
    QPushButton *videostorebtn,*photostorebtn,*gologinbtn,*returnbtn;
    QLineEdit *videopathedit,*photopathedit;
    QComboBox *collectiontimebox,*camerasebox;
    QString videoPath;
    QString photoPath;
    int collectionTimes;
    QList<QString> cameraNames;
    QString currentCamera;

};

#endif // SYSSETTINGWIDGET_H
