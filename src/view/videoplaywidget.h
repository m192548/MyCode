#ifndef VIDEOPLAYWIDGET_H
#define VIDEOPLAYWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include <QResizeEvent>
#include <QCloseEvent>
#include "model/ffmpegvideoprocessor.h"
#include <QEvent>
#include <QPoint>
#include <QEvent>
class VideoPlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayWidget(QWidget *parent = nullptr);
    ~VideoPlayWidget();
    void initUi();
    void initConnect();
    void initWin();
    void initQSS();
    void openVideoFile();
    void setCoverFilePath(const QString& filePath);
    void setVideoFilePath(const QString& filePath);
    void setVideoFrames(const int& count);
    void setPhotoPath(const QString& path);
    void savePhotoToDatabase(QString& photoName,QDateTime& saveTime);
    bool eventFilter(QObject *obj, QEvent *event)override;
    int calculateSliderValue(const QPoint &pos);
    void resetParamsAndshow();//重置参数和界面显示
    void jumpToSliderPos(int value);//跳转到滑块位置
    bool h264ToMp4(const QString& h264Path, const QString& mp4Path);
protected:
    void resizeEvent(QResizeEvent* event)override;
    void closeEvent(QCloseEvent* event)override;
signals:

public slots:
    void onPlayBtnClicked(bool checked);
    void onMultipleBtnClicked();
    void onExportBtnClicked();
    void onScreenCaptureBtnClicked();
    void onShowVideoToLabel();
    void onCurrentVideoChanged();

private:
    QWidget* topWidget;
    QWidget* midWidget;
    QWidget* durationInfoWidget;
    QWidget* bottomWidget;

    QHBoxLayout* topWidgetLayout;
    QVBoxLayout* midWidgetLayout;
    QHBoxLayout* durationInfoWidgetLayout;
    QHBoxLayout* bottomWidgetLayout;
    QVBoxLayout* totalLayout;

    QLabel* logoLabel,*titleLabel;
    QLabel* videoPlayLabel;
    QLabel* currentDurationLabel;//显示当前播放时长
    QLabel* totalDurationLabel;//显示总时长
    QSlider* videoSlider;

    QPushButton* playBtn,*multipleBtn,*exportBtn,*screenCaptureBtn;
private:
    QPixmap cover;              //封面
    QPixmap currentFrame;       //当前帧
    QString photoPath;          //保存图片目录路径
    QString videoFilePath;     //视频文件路径
    QString coverFilePath;
    double multiple;          //播放倍数
private:
    QTimer *m_playTimer;       // 播放定时器（控制帧率）
    float m_frameRate;        // 视频帧率(控制帧间隔)
    int totalFrames;           //总帧数
    int currentPlayFrames;      //当前播放帧数
    bool m_isPlaying;          // 播放状态
    float totalSeconds;          //总秒数
    float currentSeconds;         //当前播放秒数
    FFmpegVideoProcessor* videoProcessor;
    bool decodeIsInit;          //解码器是否初始化
    AVFrame* yuvFrame,*rgbFrame;
    int m_rgbWidth;
    int m_rgbHeight;
    int m_rgbBufferSize ;
    uint8_t* rgbBuffer;
};

#endif // VIDEOPLAYWIDGET_H
