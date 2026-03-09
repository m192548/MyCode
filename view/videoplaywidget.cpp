#include "videoplaywidget.h"
#include <QIcon>
#include <QFile>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QTime>
#include <QStyleOptionSlider>
#include <cmath>
#include <iostream>
#include <QDateTime>
#include <QMessageBox>
#include "model/singletondb.h"
#include <fstream>
VideoPlayWidget::VideoPlayWidget(QWidget *parent) : QWidget(parent)
{
    m_isPlaying=false;
    videoProcessor=new FFmpegVideoProcessor(this);
    decodeIsInit=false;
    m_frameRate=30;
    multiple=1.0;
    currentPlayFrames=0;
    currentSeconds=0;
    m_playTimer=new QTimer(this);
    initUi();
    initConnect();
    initWin();
    initQSS();
}
VideoPlayWidget::~VideoPlayWidget()
{

}
void VideoPlayWidget::initUi()
{
    //顶部视图
    topWidget=new QWidget(this);
    topWidget->setFixedHeight(50);
    topWidget->setObjectName("topWidget");
    topWidgetLayout=new QHBoxLayout(topWidget);
    topWidgetLayout->setSpacing(0);
    topWidgetLayout->setMargin(0);
    logoLabel=new QLabel(topWidget);
    logoLabel->setFixedHeight(50);
    logoLabel->setPixmap(QPixmap(":/images/monitor1.png").scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setObjectName("logoLabel");
    titleLabel=new QLabel("安防监控系统",topWidget);
    titleLabel->setFixedHeight(50);
    titleLabel->setObjectName("titleLabel");
    topWidgetLayout->addWidget(logoLabel);
    topWidgetLayout->addWidget(titleLabel);
    topWidgetLayout->addStretch();


    //中间视图
    midWidget=new QWidget(this);
    midWidget->setObjectName("midWidget");
    midWidgetLayout=new QVBoxLayout(midWidget);
    midWidgetLayout->setSpacing(0);
    midWidgetLayout->setMargin(0);
    videoPlayLabel=new QLabel(midWidget);
    videoPlayLabel->resize(midWidget->width(),550);
    videoPlayLabel->setObjectName("videoPlayLabel");
    videoPlayLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    videoPlayLabel->resize(1000,650);

    durationInfoWidget=new QWidget(midWidget);
    durationInfoWidget->setObjectName("durationInfoWidget");
    durationInfoWidget->setFixedHeight(50);
    durationInfoWidgetLayout=new QHBoxLayout(durationInfoWidget);
    durationInfoWidgetLayout->setSpacing(2);
    durationInfoWidgetLayout->setMargin(5);
    currentDurationLabel=new QLabel("00:00:00",durationInfoWidget);
    currentDurationLabel->setFixedHeight(50);
    currentDurationLabel->setAlignment(Qt::AlignCenter);
    currentDurationLabel->setObjectName("curentDurationLabel");
    totalDurationLabel=new QLabel("00:00:00",durationInfoWidget);
    totalDurationLabel->setObjectName("totalDurationLabel");
    totalDurationLabel->setFixedHeight(50);
    totalDurationLabel->setAlignment(Qt::AlignCenter);
    videoSlider=new QSlider(midWidget);
    videoSlider->setOrientation(Qt::Horizontal); // 显式设置水平（可省略，默认即为水平）
    videoSlider->setFixedHeight(50);
    videoSlider->setObjectName("videoSlider");
    videoSlider->installEventFilter(this);
    durationInfoWidgetLayout->addWidget(currentDurationLabel);
    durationInfoWidgetLayout->addWidget(videoSlider);
    durationInfoWidgetLayout->addWidget(totalDurationLabel);

    midWidgetLayout->addWidget(videoPlayLabel);
    midWidgetLayout->addWidget(durationInfoWidget);


    //底部视图
    bottomWidget=new QWidget(this);
    bottomWidget->setObjectName("bottomWidget");
    bottomWidget->setFixedHeight(50);
    bottomWidgetLayout=new QHBoxLayout(bottomWidget);
    playBtn=new QPushButton("播放",bottomWidget);
    playBtn->setCheckable(true);
    multipleBtn=new QPushButton("倍数:1.0x",bottomWidget);
    exportBtn=new QPushButton("导出",bottomWidget);
    screenCaptureBtn=new QPushButton("截屏",bottomWidget);

    bottomWidgetLayout->addWidget(playBtn);
     bottomWidgetLayout->setMargin(0);
    bottomWidgetLayout->addStretch();
    bottomWidgetLayout->addWidget(multipleBtn);
    bottomWidgetLayout->addWidget(exportBtn);
    bottomWidgetLayout->addWidget(screenCaptureBtn);


    totalLayout=new QVBoxLayout(this);
    totalLayout->setSpacing(0);
    totalLayout->setMargin(0);
    totalLayout->addWidget(topWidget);
    totalLayout->addWidget(midWidget);
    totalLayout->addWidget(bottomWidget);



}
void VideoPlayWidget::initConnect()
{
    connect(playBtn,SIGNAL(clicked(bool)),this,SLOT(onPlayBtnClicked(bool)));
    connect(multipleBtn,SIGNAL(clicked()),this,SLOT(onMultipleBtnClicked()));
    connect(exportBtn,SIGNAL(clicked()),this,SLOT(onExportBtnClicked()));
    connect(screenCaptureBtn,SIGNAL(clicked()),this,SLOT(onScreenCaptureBtnClicked()));
    connect(m_playTimer,SIGNAL(timeout()),this,SLOT(onShowVideoToLabel()));
}
void VideoPlayWidget::initWin()
{
    this->setWindowTitle("视频播放");
    this->setWindowIcon(QIcon(":/images/icon.png"));
    this->setMinimumSize(1000,800);
    //this->setMaximumSize(1920,1080);
    resize(1000,800);
}
void VideoPlayWidget::initQSS()
{
    //加载外部qss文件
    QFile file(":/qss/videoplaywidget.qss");//资源文件路径
    if(file.open(QFile::ReadOnly))
    {
        QString styleSheet=QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

}
void VideoPlayWidget::openVideoFile()
{
    if(!videoProcessor->openDevice(""))return;
    if(!videoProcessor->initDecoder(videoFilePath,"h264"))
    {
        qDebug()<<"初始化解码器失败";
        return;
    }
    yuvFrame=av_frame_alloc();

    if(!yuvFrame)
    {
        qDebug()<<"分配yucFrame内存失败";
        videoProcessor->closeDevice();
        return;
    }
    rgbFrame=av_frame_alloc();
    if(!rgbFrame)
    {
        qDebug()<<"分配rgbFrame内存失败";
        av_frame_free(&yuvFrame);
        yuvFrame=nullptr;
        videoProcessor->closeDevice();
        return;
    }
    m_rgbWidth = videoProcessor->getOriginalWidth();
    m_rgbHeight = videoProcessor->getOriginalHeight();
    m_rgbBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_rgbWidth, m_rgbHeight, 1);
    rgbBuffer = (uint8_t*)av_malloc(m_rgbBufferSize);
    if (m_rgbBufferSize <= 0)
    {
      qDebug()<<"计算RGB缓冲区大小失败";
      av_frame_free(&yuvFrame);
      yuvFrame=nullptr;
      av_frame_free(&rgbFrame);
      rgbFrame=nullptr;
      return;
    }
    decodeIsInit=true;

}
void VideoPlayWidget::setCoverFilePath(const QString& filePath)
{
    coverFilePath=filePath;
    currentFrame.load(coverFilePath);
}
void VideoPlayWidget::setVideoFilePath(const QString& filePath)
{
    videoFilePath=filePath;
}
void VideoPlayWidget::setVideoFrames(const int& count)
{
    totalFrames=count;
}
void VideoPlayWidget::setPhotoPath(const QString& path)
{
    photoPath=path;
}
void VideoPlayWidget::savePhotoToDatabase(QString& photoName,QDateTime& saveDateTime)
{

    QString sql=QString("insert into tbl_photo (photo_name,photo_path,photo_time)"
            "values(\"%1\",\"%2\",\"%3\")")
            .arg(photoName).arg(photoPath+"/").arg(saveDateTime.toString("yyyy-MM-dd HH:mm:ss"));
    if(0!=SingletonDB::getInstance()->execsql(sql))
    {
            qDebug()<<"保存图片:"<<photoName<<"到数据库失败";
    }
}
// 事件过滤器
bool VideoPlayWidget::eventFilter(QObject *obj, QEvent *event)
{
       if (obj == videoSlider && event->type() == QEvent::MouseButtonPress)
       {
           QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
           if (mouseEvent->button() == Qt::LeftButton)
           {
               // 计算点击位置对应的滑块值（Qt5.9.8兼容）
               int value = calculateSliderValue(mouseEvent->pos());
               qDebug() << "点击位置值：" << value;
               jumpToSliderPos(value);
               return true;
           }
       }
       // 其他事件交给父类处理
       return QWidget::eventFilter(obj, event);
}
// slider点击坐标
int VideoPlayWidget::calculateSliderValue(const QPoint &pos)
{

    int min = videoSlider->minimum();
    int max = videoSlider->maximum();


    QStyleOptionSlider opt;
    opt.initFrom(videoSlider);
    opt.orientation = videoSlider->orientation();
    opt.minimum = min;
    opt.maximum = max;
    opt.sliderPosition = videoSlider->value();
    opt.sliderValue = videoSlider->value();
    opt.singleStep = videoSlider->singleStep();
    opt.pageStep = videoSlider->pageStep();
    opt.tickPosition = videoSlider->tickPosition();
    opt.tickInterval = videoSlider->tickInterval();
    opt.upsideDown = videoSlider->invertedAppearance(); // Qt5.9.8支持：反转外观

    // 获取滑块轨道和手柄的矩形区域
    QRect grooveRect = videoSlider->style()->subControlRect(
        QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, videoSlider);
    QRect handleRect = videoSlider->style()->subControlRect(
        QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, videoSlider);

    int value = min;
    if (videoSlider->orientation() == Qt::Horizontal)
    {
        // 水平滑块：计算点击位置
        int x = pos.x() - grooveRect.x() - handleRect.width() / 2;
        int sliderLength = grooveRect.width() - handleRect.width();
        value = QStyle::sliderValueFromPosition(min, max, x, sliderLength, opt.upsideDown);
    }
    value = qBound(min, value, max);
    return value;
}
//重置参数和界面显示信息
void VideoPlayWidget::resetParamsAndshow()
{
    //重新计算总时长
    totalSeconds=totalFrames/m_frameRate;//总秒数
    multiple=1.0;
    multipleBtn->setText("倍数:1.0x");
    //当前秒数
    currentSeconds=0;
    //重置滑块
    videoSlider->setRange(0,totalSeconds);
    videoSlider->setValue(0);

    //重置界面显示
    videoPlayLabel->setPixmap(currentFrame.scaled(videoPlayLabel->size()));
    //重置时长标签文本
    totalDurationLabel->setText("00:00:00");
    currentDurationLabel->setText("00:00:00");

    QTime totalTimes=QTime::fromString("00:00:00").addSecs(totalSeconds);
    totalDurationLabel->setText(totalTimes.toString());
    qDebug()<<"帧数:"<<totalFrames<<"总秒数:"<<totalSeconds;
}

void VideoPlayWidget::jumpToSliderPos(int value)//跳转到滑块位置
{
    currentSeconds=value;
    currentPlayFrames=currentSeconds*m_frameRate;
    videoProcessor->closeDevice();
    //重新打开文件从开始位置编码到当前帧
    openVideoFile();
    int frameCnt=0;
    while(true)
    {
        if(videoProcessor->readAndDecodeFrame(yuvFrame))
        {
            if (videoProcessor->convertToRGB(yuvFrame, m_rgbWidth, m_rgbHeight,rgbBuffer,rgbFrame))
            {
                frameCnt++;
                if(frameCnt>=currentPlayFrames)
                {
                    QImage rgbImg(rgbBuffer, m_rgbWidth, m_rgbHeight, QImage::Format_RGB888);
                    currentFrame=QPixmap::fromImage(rgbImg);
                    videoPlayLabel->setPixmap(currentFrame.scaled(videoPlayLabel->size()));
                 break;
                }
            }
        }

    }
   //让滑块跳转到点击位置（可选）
   videoSlider->setValue(value);
   QTime playTimes=QTime::fromString("00:00:00").addSecs(currentSeconds);
   currentDurationLabel->setText(playTimes.toString());
}
// H.264 封装为 MP4
bool VideoPlayWidget::h264ToMp4(const QString& h264Path, const QString& mp4Path)
{
    //注册 FFmpeg 组件
    avformat_network_init();
    AVFormatContext* inFmtCtx = nullptr;
    AVFormatContext* outFmtCtx = nullptr;
    AVStream* outStream = nullptr;
    //打开输入文件
    AVInputFormat* inFmt = av_find_input_format("h264");
    if (avformat_open_input(&inFmtCtx, h264Path.toUtf8().constData(), inFmt, nullptr) < 0)
    {
        qDebug() << "打开 H.264 文件失败";
        // 释放资源
        if (inFmtCtx) avformat_close_input(&inFmtCtx);
        if (outFmtCtx && !(outFmtCtx->oformat->flags & AVFMT_NOFILE))
            avio_close(outFmtCtx->pb);
        if (outFmtCtx) avformat_free_context(outFmtCtx);
    }

    //查找流信息
    if (avformat_find_stream_info(inFmtCtx, nullptr) < 0)
    {
        qDebug() << "查找流信息失败";
        AVStream* inStream = avformat_new_stream(inFmtCtx, nullptr);
        inStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        inStream->codecpar->codec_id = AV_CODEC_ID_H264;
        inStream->codecpar->width = 1280;
        inStream->codecpar->height = 720;
    }
    // 找到视频流索引
    int videoIndex = -1;
    for (int i = 0; i < inFmtCtx->nb_streams; i++)
    {
        if (inFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoIndex = i;
            break;
        }
    }
    if (videoIndex < 0)
    {
        qDebug() << "未找到视频流";
        // 释放资源
        if (inFmtCtx) avformat_close_input(&inFmtCtx);
        if (outFmtCtx && !(outFmtCtx->oformat->flags & AVFMT_NOFILE))
            avio_close(outFmtCtx->pb);
        if (outFmtCtx) avformat_free_context(outFmtCtx);
    }
    AVStream* inStream = inFmtCtx->streams[videoIndex];

    //创建 MP4 输出上下文
    if (avformat_alloc_output_context2(&outFmtCtx, nullptr, "mp4", mp4Path.toUtf8().constData()) < 0)
    {
        qDebug() << "创建 MP4 输出上下文失败";
        // 释放资源
        if (inFmtCtx) avformat_close_input(&inFmtCtx);
        if (outFmtCtx && !(outFmtCtx->oformat->flags & AVFMT_NOFILE))
            avio_close(outFmtCtx->pb);
        if (outFmtCtx) avformat_free_context(outFmtCtx);
    }

    //添加视频流到 MP4
    outStream = avformat_new_stream(outFmtCtx, nullptr);
    if (!outStream)
    {
        qDebug() << "创建输出流失败";
        // 释放资源
        if (inFmtCtx) avformat_close_input(&inFmtCtx);
        if (outFmtCtx && !(outFmtCtx->oformat->flags & AVFMT_NOFILE))
            avio_close(outFmtCtx->pb);
        if (outFmtCtx) avformat_free_context(outFmtCtx);
    }
    // 复制输入流参数到输出流
    if (avcodec_parameters_copy(outStream->codecpar, inStream->codecpar) < 0)
    {
        qDebug() << "复制流参数失败";
        // 释放资源
        if (inFmtCtx) avformat_close_input(&inFmtCtx);
        if (outFmtCtx && !(outFmtCtx->oformat->flags & AVFMT_NOFILE))
            avio_close(outFmtCtx->pb);
        if (outFmtCtx) avformat_free_context(outFmtCtx);
    }
    outStream->codecpar->codec_tag = 0;

    //设置时间基（关键：帧率转时间基）
    AVRational timeBase = {1, int(m_frameRate)}; // 时间基 = 1/帧率
    outStream->time_base = timeBase;
    inStream->time_base = timeBase;

    // 8. 打开 MP4 输出文件
    if (!(outFmtCtx->oformat->flags & AVFMT_NOFILE))
    {
        if (avio_open(&outFmtCtx->pb, mp4Path.toUtf8().constData(), AVIO_FLAG_WRITE) < 0)
        {
            qDebug() << "打开 MP4 输出文件失败";
            // 释放资源
            if (inFmtCtx) avformat_close_input(&inFmtCtx);
            if (outFmtCtx && !(outFmtCtx->oformat->flags & AVFMT_NOFILE))
                avio_close(outFmtCtx->pb);
            if (outFmtCtx) avformat_free_context(outFmtCtx);
        }
    }

    //写入 MP4 头信息
    if (avformat_write_header(outFmtCtx, nullptr) < 0)
    {
        qDebug() << "写入头信息失败";
        // 释放资源
        if (inFmtCtx) avformat_close_input(&inFmtCtx);
        if (outFmtCtx && !(outFmtCtx->oformat->flags & AVFMT_NOFILE))
            avio_close(outFmtCtx->pb);
        if (outFmtCtx) avformat_free_context(outFmtCtx);
    }

    //逐帧读取 H.264 数据包并写入 MP4
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    int frameCount = 0; // 帧计数，用于计算时间戳
    while (av_read_frame(inFmtCtx, &pkt) >= 0)
    {
        if (pkt.stream_index != videoIndex)
        {
            av_packet_unref(&pkt);
            continue;
        }

        // 设置时间戳
        frameCount++;
        pkt.pts = av_rescale_q(frameCount, timeBase, outStream->time_base);
        pkt.dts = pkt.pts; // H.264 裸流无 B 帧时，dts = pts
        pkt.duration = av_rescale_q(1, timeBase, outStream->time_base);
        pkt.stream_index = 0;

        // 写入数据包到 MP4
        if (av_interleaved_write_frame(outFmtCtx, &pkt) < 0)
        {
            qDebug() << "写入数据包失败";
            av_packet_unref(&pkt);
            // 释放资源
            if (inFmtCtx) avformat_close_input(&inFmtCtx);
            if (outFmtCtx && !(outFmtCtx->oformat->flags & AVFMT_NOFILE))
                avio_close(outFmtCtx->pb);
            if (outFmtCtx) avformat_free_context(outFmtCtx);
        }
        av_packet_unref(&pkt);
    }

    //写入 MP4 尾信息
    av_write_trailer(outFmtCtx);

    // 释放资源
    if (inFmtCtx) avformat_close_input(&inFmtCtx);
    if (outFmtCtx && !(outFmtCtx->oformat->flags & AVFMT_NOFILE))
        avio_close(outFmtCtx->pb);
    if (outFmtCtx) avformat_free_context(outFmtCtx);
    return frameCount > 0;
}

void VideoPlayWidget::onPlayBtnClicked(bool checked)
{
    if(checked)
    {
        if(!decodeIsInit)
        {
            openVideoFile();
            resetParamsAndshow();
        }
        m_isPlaying=true;
        playBtn->setText("暂停");
        m_playTimer->start(1000/(m_frameRate*multiple));
    }
    else
    {
        m_isPlaying=false;
        playBtn->setText("播放");
        m_playTimer->stop();
        if(!currentFrame.isNull())
        videoPlayLabel->setPixmap(currentFrame.scaled(videoPlayLabel->size()));
    }


}
void VideoPlayWidget::onMultipleBtnClicked()
{
    //浮点数比较相等方式
    if(std::fabs(multiple-0.5)<1e-5)
    {
        multiple=1.0;
        multipleBtn->setText("倍数:1.0x");
    }
    else if(std::fabs(multiple-1.0)<1e-5)
    {
        multiple=2.0;
        multipleBtn->setText("倍数:2.0x");
    }
    else if(std::fabs(multiple-2.0)<1e-5)
    {
        multiple=3.0;
        multipleBtn->setText("倍数:3.0x");
    }
    else if(std::fabs(multiple-3.0)<1e-5)
    {
        multiple=0.5;
        multipleBtn->setText("倍数:0.5x");
    }
    if(m_isPlaying)
    {
        m_playTimer->stop();
        m_playTimer->start(1000.0/(m_frameRate*multiple));
    }

}
void VideoPlayWidget::onExportBtnClicked()
{
    int pos=videoFilePath.lastIndexOf(".");
    QString mp4FilePath=videoFilePath.left(pos)+".mp4";
    exportBtn->setText("导出中...");
    if(h264ToMp4(videoFilePath,mp4FilePath))
    {
     QMessageBox::information(this,"导出","导出成功:"+mp4FilePath);
    }
    else
    {
       QMessageBox::information(this,"导出","导出失败:"+mp4FilePath);
    }
    exportBtn->setText("导出");
}
void VideoPlayWidget::onScreenCaptureBtnClicked()
{
    if(currentFrame.isNull())
    {
        QMessageBox::information(this,"保存","保存失败");
        return;
    }
    //保存当前帧
    QDateTime currentTime=QDateTime::currentDateTime();
    QString photoName=currentTime.toString("yyyyMMddHHmmss")+".png";
    QString filePath=photoPath+"/"+photoName;
    if(currentFrame.save(filePath))
    {
        QMessageBox::information(this,"保存","保存成功:"+filePath);
        savePhotoToDatabase(photoName,currentTime);
        return;
    }
    QMessageBox::information(this,"保存","保存失败:"+filePath);

}
//视频显示槽函数
void VideoPlayWidget::onShowVideoToLabel()
{
    if(videoProcessor->readAndDecodeFrame(yuvFrame))
    {
        if (videoProcessor->convertToRGB(yuvFrame, m_rgbWidth, m_rgbHeight,rgbBuffer,rgbFrame))
        {
            currentPlayFrames++;
            QImage rgbImg(rgbBuffer, m_rgbWidth, m_rgbHeight, QImage::Format_RGB888);
            currentFrame=QPixmap::fromImage(rgbImg);
            videoPlayLabel->setPixmap(currentFrame.scaled(videoPlayLabel->size()));

            //更新时长
            if(currentPlayFrames%25==0)
            {
             currentSeconds+=1;
             QTime playTimes=QTime::fromString("00:00:00").addSecs(currentSeconds<totalSeconds?currentSeconds:totalSeconds);
             currentDurationLabel->setText(playTimes.toString());
             videoSlider->setValue((int)currentSeconds);
            }
        }

    }
    else
    {
        qDebug()<<"视频播放结束";
        playBtn->setChecked(false);
        playBtn->setText("播放");

        m_playTimer->stop();
        m_isPlaying=false;
        videoProcessor->closeDevice();
        decodeIsInit=false;
        av_frame_free(&yuvFrame);
        yuvFrame=nullptr;
        av_frame_free(&rgbFrame);
        rgbFrame=nullptr;
        av_free(rgbBuffer);
        rgbBuffer=nullptr;
    }
}
void VideoPlayWidget::onCurrentVideoChanged()
{
    //当切换视频的时候如果正在播放则先暂停
    if(m_isPlaying)
    {
        m_isPlaying=false;
        m_playTimer->stop();
        playBtn->setChecked(false);
        playBtn->setText("播放");
    }
    videoProcessor->closeDevice();
    resetParamsAndshow();
    openVideoFile();

}
void VideoPlayWidget::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    videoPlayLabel->setPixmap(currentFrame.scaled(videoPlayLabel->size()));
}
void VideoPlayWidget::closeEvent(QCloseEvent* event)
{   
    if(m_isPlaying)
    {
        m_playTimer->stop();
        playBtn->setChecked(false);
        playBtn->setText("播放");
        m_isPlaying=false;
    }
    currentFrame=QPixmap();
    videoProcessor->closeDevice();
    QWidget::closeEvent(event);

}
