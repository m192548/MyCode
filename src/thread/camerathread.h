#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QObject>
#include <QThread>
#include <QImage>
#include <QVector>
#include <QString>
#include <QQueue>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include "model/ffmpegvideoprocessor.h"
#include<QMutex>
#include <fstream>

class CameraThread : public QThread
{
    Q_OBJECT
public:
    explicit CameraThread(QObject* parent = nullptr);
    ~CameraThread();
    // 打开摄像头
    bool openCamera(const QString& deviceName,int width = 1280, int height = 720, int fps = 30);
    // 停止捕获
    void stopCamera();
    void setVideoPath(const QString& videoPath);
    void setCollectTimes(const int& collectTimes);
    void startThread();
private:
    void addVideoToDataBase();
public slots:
    void onSaveVideo();//保存视频，将视频最后的帧写入文件
signals:
    // 新帧就绪信号（同时提供原始YUV帧和RGB显示帧）
    void frameReady(const QImage& rgbFrame,bool isCover,const QString& coverFileName);
    void errorOccurred(const QString& errorMsg);
    void newVideoBuild(const QString videoName,const QString& coverName,const int& framesCount);
protected:
    void run()override;

private:
    void cacheFrame(AVFrame* frame);

private:
    bool m_running = false;
    FFmpegVideoProcessor* m_processor = nullptr; // 组合FFmpeg功能类s
    QTimer collectTimer;//定时采集视频
    QQueue<AVFrame*>cachedFrames;
    QMutex mutex;

    QDateTime startCollectTime;
    std::ofstream output_file;
    QString currentVideoPath;   //视频文件路径(目录)
    int collectTimes;
    QString videoFilePath;      //视频文件路径名(目录+文件名)
    QString coverFilePath;      //封面文件路径名(目录+文件名)
    QString coverName;          //视频文件名
    QString videoName;          //封面文件名
    int curVideoFrameCount;
    bool isCover;//当前帧是否是封面
    bool isCompleteVideo;
};
#endif // CAMERATHREAD_H
