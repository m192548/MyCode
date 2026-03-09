#include "camerathread.h"
#include <QtDebug>
#include "model/singletondb.h"
#include <chrono>
#include <QFile>

CameraThread::CameraThread(QObject* parent)
    : QThread(parent)
{
    m_processor = new FFmpegVideoProcessor();

    collectTimes=0;
    curVideoFrameCount=0;
    isCover=true;
    isCompleteVideo=false;
    connect(&collectTimer,SIGNAL(timeout()),this,SLOT(onSaveVideo()));
}

CameraThread::~CameraThread()
{
    stopCamera();
    delete m_processor;
}
void CameraThread::setVideoPath(const QString& videoPath)
{
     this->currentVideoPath=videoPath;
}
void CameraThread::setCollectTimes(const int& collectTimes)
{
    this->collectTimes=collectTimes;
}
void CameraThread::startThread()
{
    this->start();
    collectTimer.start(collectTimes*1000);
}
// 打开摄像头
bool CameraThread::openCamera(const QString& deviceName,int width, int height,int fps)
{
    if(m_processor->openDevice(deviceName, width, height, fps))
    {
        if(m_processor->initDecoder(deviceName,"dshow"))
            return true;

    }

    return false;
}

// 停止捕获
void CameraThread::stopCamera()
{
    m_running = false;
}
void CameraThread::cacheFrame(AVFrame* frame)
{
    if (!frame) return;
    frame->pts=av_gettime();
    QMutexLocker locker(&mutex);

    curVideoFrameCount++;

     if(m_processor->encodeFrame(frame))
     {
         m_processor->writeFrame(output_file);
     }
     else
     {
        qDebug()<<"编码第"<<curVideoFrameCount<<"帧失败";
     }
     av_frame_free(&frame);
     frame=nullptr;

}
void CameraThread::onSaveVideo()
{
    QMutexLocker locker(&mutex);
     m_processor->flush_encoder(output_file);

     //重置编码器和输出文件
     m_processor->cleanupEncoder();
     output_file.close();
     //重命名文件(去除.cache)
     QFile videoFile(videoFilePath);
     int pos=videoFilePath.lastIndexOf(".");
     videoFilePath=videoFilePath.left(pos);
     videoFile.rename(videoFilePath);
     QFile coverFile(coverFilePath);
     pos=coverFilePath.lastIndexOf(".");
     coverFilePath=coverFilePath.left(pos);
     coverFile.rename(coverFilePath);
     //写入数据库
     addVideoToDataBase();
     emit newVideoBuild(videoName.left(videoName.lastIndexOf(".")),
                        coverName.left(coverName.lastIndexOf(".")),
                        curVideoFrameCount);


    startCollectTime=QDateTime::currentDateTime();
    qDebug()<<"时间:"<<startCollectTime<<"帧数:"<<curVideoFrameCount;
     curVideoFrameCount=0;
    videoName=startCollectTime.toString("yyyyMMddHHmmss")+".h264.cache";
    coverName=startCollectTime.toString("yyyyMMddHHmmss")+".png.cache";

    videoFilePath=currentVideoPath+'/'+videoName;
    coverFilePath=currentVideoPath+"/cover/"+coverName;

    output_file.open(videoFilePath.toStdString(),std::ios::binary|std::ios::app);
    if(!output_file.is_open())
    {
        qDebug()<<"无法创建视频文件";
        return;
    }
    if(!m_processor->initEncoder(output_file))
    {
        qDebug()<<"初始化编码器失败";
    }
    isCover=true;

}
 void CameraThread::addVideoToDataBase()
 {
     //写入数据库
     QString _videoName=videoName.left(19);
     QString _coverName=coverName.left(18);
     QString videoTime=startCollectTime.toString("yyyy-MM-dd HH:mm:ss");
      QString sql=QString("insert into tbl_video"
                          "(video_name,video_path,video_cover,video_total_frame,video_time)"
                          "values(\"%1\",\"%2\",\"%3\",%4,\"%5\")")
              .arg(_videoName).arg(currentVideoPath+"/").arg(_coverName).arg(curVideoFrameCount).arg(videoTime);
     if(!SingletonDB::getInstance()->execsql(sql)==0)
     {
         qDebug() << "写入数据库失败";
     }
 }
void CameraThread::run()
{
    //初始化数据
    m_running = true;

    startCollectTime=QDateTime::currentDateTime();

    //初始化视频和封面名，文件路径名
    videoName=startCollectTime.toString("yyyyMMddHHmmss")+".h264.cache";
    coverName=startCollectTime.toString("yyyyMMddHHmmss")+".png.cache";
    videoFilePath=currentVideoPath+'/'+videoName;
    coverFilePath=currentVideoPath+"/cover/"+coverName;

    qDebug()<<"时间:"<<startCollectTime;
    output_file.open(videoFilePath.toStdString(),std::ios::binary|std::ios::app);
    if(!output_file.is_open())
    {
        qDebug()<<"无法创建视频文件";
        return;
    }
    if(!m_processor->initEncoder(output_file))
    {
        qDebug()<<"初始化编码器失败";
    }
    AVFrame* yuvFrame=av_frame_alloc();
    AVFrame* rgbFrame=av_frame_alloc();
    if (!yuvFrame || !rgbFrame)
    {
      qDebug()<<"yuvFrame,rgbFrame内存分配失败";
      m_running = false;
      return;
    }
    int rgbWidth = m_processor->getOriginalWidth();
    int rgbHeight = m_processor->getOriginalHeight();
    int rgbBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, rgbWidth, rgbHeight, 1);
    uint8_t* rgbBuffer = (uint8_t*)av_malloc(rgbBufferSize);
    if (rgbBufferSize <= 0)
    {
      qDebug()<<"计算RGB缓冲区大小失败";
      m_running = false;
      av_frame_free(&yuvFrame);
      av_frame_free(&rgbFrame);
      return;
    }
    //循环读取视频流
    while (this->m_running)
    {
        if (!m_processor->readAndDecodeFrame(yuvFrame))
        {
            emit errorOccurred("读取帧失败");
            QThread::msleep(10);
            continue;
        }
        //转rgb

        if (!m_processor->convertToRGB(yuvFrame, rgbWidth, rgbHeight,rgbBuffer,rgbFrame))
        {
            QThread::msleep(10);
            emit errorOccurred("格式转换失败");
            continue;
        }

        // 发送信号
        QImage rgbImg(rgbBuffer, rgbWidth, rgbHeight, QImage::Format_RGB888);
       QImage copyImg=rgbImg.copy();
        if (rgbImg.isNull())
        {
            qDebug()<<"QImage构造失败：内存不足";
            continue;
         }
        AVFrame* cloneFrame=av_frame_clone(yuvFrame);

        cacheFrame(cloneFrame);
        if(isCover)
        {
            emit frameReady(copyImg,true,coverName);
            isCover=false;
        }
        else
        {
            emit frameReady(copyImg,false,QString());
        }

    }

    // 清理
    av_frame_free(&yuvFrame);
    yuvFrame=nullptr;
    av_frame_free(&rgbFrame);
    rgbFrame=nullptr;
    av_free(rgbBuffer);
    rgbBuffer=nullptr;
}
