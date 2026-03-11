#ifndef FFMPEG_VIDEO_PROCESSOR_H
#define FFMPEG_VIDEO_PROCESSOR_H

#include <QObject>
#include <QString>
// FFmpeg头文件
extern "C"
{
    #include "libavcodec/avcodec.h"              //编解码库，最重要的模块之一
    #include "libavdevice/avdevice.h"            //输入输出设备库；读取摄像头的
    #include "libavfilter/avfilter.h"            //音视频滤镜库；进行音视频处理与编辑
    #include "libavformat/avformat.h"            //文件格式和协议库
    #include "libavutil/avutil.h"                //音视频处理
    #include <libavutil/imgutils.h>
    #include "libswresample/swresample.h"       //音频重采样
    #include "libswscale/swscale.h"            //图像进行格式转换
    #include <libavutil/time.h>
    #include <libavfilter/buffersrc.h>

}

class FFmpegVideoProcessor :public QObject{
public:
    FFmpegVideoProcessor(QObject *parent=nullptr);
    ~FFmpegVideoProcessor();

    // 打开摄像头设备
    bool openDevice(const QString& deviceName,
                   int width = 1280, int height = 720, int fps = 30);

    // 关闭设备并释放资源
    void closeDevice();

    // 获取原始视频宽高
    int getOriginalWidth() const;
    int getOriginalHeight() const;
    // 设备状态查询
    bool isDeviceOpen() const { return m_isDeviceOpen; }
    //获取并转到到指定秒数的帧
    bool seekToSec(double sec);
    // 编码相关
    bool initEncoder(std::ofstream& output_file);
    bool encodeFrame(AVFrame* frame);
    void writeFrame(std::ofstream& output_file);
    void cleanupEncoder();

    //解码相关
    bool initDecoder(const QString& intputFile,const QString& shortName);
    bool decodeFrame(AVFrame*& yuvFrame);
    void cleanupDecoder();
    bool readAndDecodeFrame(AVFrame*& yuvFrame);
    bool convertToRGB(const AVFrame* yuvFrame,int rgbWidth, int rgbHeight,uint8_t* rgbBuffer,AVFrame*& rgbFrame);
    void flush_encoder(std::ofstream& output_file);



private:

    // 设备参数
    QString m_deviceName;
    int m_width = 1280;
    int m_height = 720;
    int m_fps = 30;
    bool m_isDeviceOpen = false;

    AVFormatContext* m_inputFmtCtx = nullptr;    // 输入格式
    AVCodecContext* m_decodecCtx = nullptr;      // 解码器
    AVStream* m_videoInputStream = nullptr;      // 视频流
    SwsContext* m_decodeSwsCtx = nullptr;            // 用于格式转换
    AVPacket* m_decode_pkt=nullptr;                // 解码输入包

    AVFormatContext* m_outputFmtCtx = nullptr;   // 输出格式
    AVCodecContext* m_encodecCtx = nullptr;        // 编码器
    AVStream* m_videoOutputStream = nullptr;     // 视频流
    SwsContext* m_encodeSwsCtx = nullptr;        // 用于格式转换
    AVPacket* m_encode_pkt=nullptr;                // 编码输入包

    int m_videoStreamIndex = -1;
    int64_t first_pts;                           //当前视频首帧的时间戳

};
#endif //FFMPEG_VIDEO_PROCESSOR_H
