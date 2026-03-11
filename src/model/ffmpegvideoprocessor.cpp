#include "ffmpegvideoprocessor.h"
#include <QDebug>
#include <QQueue>
#include "singletondb.h"
#include <QDateTime>
#include <fstream>
FFmpegVideoProcessor::FFmpegVideoProcessor(QObject *parent)
    :QObject(parent)
{
    avdevice_register_all();
    first_pts=0;
}

FFmpegVideoProcessor::~FFmpegVideoProcessor() {
    closeDevice();
}

bool FFmpegVideoProcessor::openDevice(const QString& deviceName,int width, int height, int fps)
{
    if (m_isDeviceOpen)
    {
        qDebug() << "设备已打开";
        return false;
    }

    // 保存设备参数
    m_deviceName = deviceName;
    m_width = width;
    m_height = height;
    m_fps = fps;
    m_isDeviceOpen=true;
    return true;

}

void FFmpegVideoProcessor::closeDevice()
{
    if (!m_isDeviceOpen) return;

    cleanupDecoder();
    m_isDeviceOpen = false;
}


// 初始化FFmpeg编码器和输出上下文
bool FFmpegVideoProcessor::initEncoder(std::ofstream& output_file)
{
        first_pts = 0;
        const AVCodec* coder = avcodec_find_encoder_by_name("libx264");
        if (!coder)
        {
            qDebug()<<"查找H.264编码器失败";
            avformat_free_context(m_outputFmtCtx);
            m_outputFmtCtx = nullptr;
            return false;
        }

        m_encodecCtx = avcodec_alloc_context3(coder);
        if (!m_encodecCtx)
        {
            qDebug()<<"分配编码器上下文失败";
            avformat_free_context(m_outputFmtCtx);
            m_outputFmtCtx = nullptr;
            m_videoOutputStream = nullptr;
            return false;
        }

        // 编码器参数配置
       m_encodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
       m_encodecCtx->codec_id = AV_CODEC_ID_H264;
       m_encodecCtx->level = 40;
       m_encodecCtx->refs = 1;
       m_encodecCtx->width = m_width;
       m_encodecCtx->height = m_height;
       m_encodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
       m_encodecCtx->time_base = (AVRational){1, m_fps};
       m_encodecCtx->framerate = (AVRational){m_fps, 1};
       m_encodecCtx->bit_rate = 2000000;
       m_encodecCtx->gop_size = 30;
       m_encodecCtx->max_b_frames = 0;


        // 设置x264参数
        av_opt_set(m_encodecCtx->priv_data, "preset", "fast", 0);
        av_opt_set(m_encodecCtx->priv_data, "tune", "zerolatency", 0);
        av_opt_set(m_encodecCtx->priv_data, "crf", "23", 0);

        // 打开编码器（检查返回值）
        int ret = avcodec_open2(m_encodecCtx, coder, nullptr);
        if (ret < 0)
        {
            qDebug()<<"打开编码器失败:"<<ret;
            avcodec_free_context(&m_encodecCtx);
            avformat_free_context(m_outputFmtCtx);
            m_videoOutputStream = nullptr;
            m_outputFmtCtx = nullptr;
            return false;
        }
        if (m_encodecCtx->extradata_size == 0)
        {
            qDebug()<<"编码器未生成SPS/PPS";
            cleanupEncoder();
            return false;
        }

           if (m_encodecCtx->extradata_size > 0) {
               output_file.write(reinterpret_cast<char*>(m_encodecCtx->extradata), m_encodecCtx->extradata_size);
           }

        // 创建SwsContext（明确输入格式，比如RGB24）
        m_encodeSwsCtx = sws_getContext(
            m_encodecCtx->width, m_encodecCtx->height, AV_PIX_FMT_YUVJ422P,
            m_encodecCtx->width, m_encodecCtx->height, m_encodecCtx->pix_fmt,
            SWS_BILINEAR, nullptr, nullptr, nullptr
        );
        if (!m_encodeSwsCtx) {
            qDebug()<<"创建m_codeSwsCtx失败";
            cleanupEncoder();
            return false;
        }
        return true;
 }
bool FFmpegVideoProcessor::encodeFrame(AVFrame* frame)
{
    AVFrame* dstFrame = frame;
    int ret = 0;
    if(first_pts==0)
    {
        first_pts = frame->pts;
    }
    // 计算相对时间戳
    int64_t framePts= av_rescale_q(
    frame->pts  - first_pts,
    (AVRational){1, 1000000},
    m_encodecCtx->time_base);
    //格式不同则转换格式
    if (m_encodecCtx->pix_fmt != frame->format)
    {
        dstFrame = av_frame_alloc();
        if (!dstFrame)
        {
            qDebug() << "分配dstFrame失败";
            return false;
        }
        dstFrame->width = frame->width;
        dstFrame->height = frame->height;
        dstFrame->format = m_encodecCtx->pix_fmt;

        dstFrame->pts = framePts;
        // 分配缓冲区
        ret=av_frame_get_buffer(dstFrame, 0);
        if ( ret< 0)
        {
            qDebug() << "分配dstFrame缓冲区失败："<<ret;
            av_frame_unref(dstFrame);
            return false;
        }

         // 格式转换
        if (!m_encodeSwsCtx) {
            qDebug() << "SwsContext未初始化";
            av_frame_unref(dstFrame);
            return false;
        }
        ret = sws_scale(
            m_encodeSwsCtx,
            frame->data, frame->linesize,
            0, frame->height,
            dstFrame->data, dstFrame->linesize
        );

        if (ret <= 0) {
            qDebug() << "转换格式失败,返回值=" << ret;
            av_frame_unref(dstFrame);
            return false;
        }
    }

     // 发送帧到编码器
    ret = avcodec_send_frame(m_encodecCtx, dstFrame);
    if (ret < 0) {
        qDebug() << "发送帧到编码器失败:" << ret;
        if (dstFrame != frame)
        {
            av_frame_unref(dstFrame);
        }
        return false;
    }
        // 分配AVPacket
       m_encode_pkt = av_packet_alloc();
       if (!m_encode_pkt)
       {
           qDebug() << "分配AVPacket失败";
           av_frame_unref(dstFrame);
           return false;
       }
        // 接收编码后的包
        while (true)
        {
            ret = avcodec_receive_packet(m_encodecCtx, m_encode_pkt);
            if (ret == AVERROR(EAGAIN))
            {
                qDebug()<<"临时无包";
                if (dstFrame != frame) {
                    av_frame_unref(dstFrame);
                }
                av_packet_unref(m_encode_pkt);
                return true;
            } else if(ret == AVERROR_EOF)
            {
                qDebug()<<"接收包失败:"<<ret;
                break;
            }


            if (ret < 0) {
                qDebug() << "接收packet失败:" <<ret;
                break;
            }
            if(ret==0)
            {
                if (dstFrame != frame)
                {
                    av_frame_unref(dstFrame);
                }

                return true;
            }
        }

        // 释放转换后的帧（若有）
        if (dstFrame != frame) {
            av_frame_unref(dstFrame);
        }
        av_packet_unref(m_encode_pkt);
        return false;
}
void FFmpegVideoProcessor::writeFrame(std::ofstream& output_file)
{
  output_file.write(reinterpret_cast<char*>(m_encode_pkt->data), m_encode_pkt->size);
  av_packet_unref(m_encode_pkt);
}


bool FFmpegVideoProcessor::initDecoder(const QString& intputFile,const QString& shortName)
{
    // 设置输入格式和设备参数
    AVInputFormat* inputFmt = nullptr;
    AVDictionary* options = nullptr;
    inputFmt = av_find_input_format(shortName.toStdString().c_str());
    av_dict_set(&options, "video_size", (std::to_string(m_width) + "x" + std::to_string(m_height)).c_str(), 0);
    av_dict_set(&options, "framerate", std::to_string(m_fps).c_str(), 0);

    //打开输入设备
    m_inputFmtCtx = avformat_alloc_context();
    //自动探寻格式
    int ret=avformat_open_input(&m_inputFmtCtx, intputFile.toStdString().c_str(), inputFmt, &options);
    if ( ret< 0)
    {
        qDebug() << "无法打开设备: " << intputFile<<","<<ret;

        return false;
    }
    av_dict_free(&options);

    //查找流信息
    if (avformat_find_stream_info(m_inputFmtCtx, nullptr) < 0) {
        qDebug() << "无法获取流信息";
        avformat_free_context(m_inputFmtCtx);
        m_inputFmtCtx = nullptr;
        return false;
    }

    //查找视频流
    for (unsigned int i = 0; i < m_inputFmtCtx->nb_streams; i++) {
        if (m_inputFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_videoStreamIndex = i;
            m_videoInputStream = m_inputFmtCtx->streams[i];
            break;
        }
    }
    if (m_videoStreamIndex == -1) {
        qDebug() << "未找到视频流";
        avformat_free_context(m_inputFmtCtx);
        m_inputFmtCtx = nullptr;
        return false;
    }

    //查找解码器
    const AVCodec* codec = avcodec_find_decoder(m_videoInputStream->codecpar->codec_id);
    if (!codec) {
        qDebug()<< "找不到解码器， codec_id=" << m_videoInputStream->codecpar->codec_id;
        avformat_free_context(m_inputFmtCtx);
        m_inputFmtCtx = nullptr;
        return false;
    }

    //创建解码器
    m_decodecCtx = avcodec_alloc_context3(codec);
    if (!m_decodecCtx) {
        qDebug()<< "无法分配解码器上下文";
        avformat_free_context(m_inputFmtCtx);
        m_inputFmtCtx = nullptr;
        return false;
    }

    //复制流参数到解码器
    if (avcodec_parameters_to_context(m_decodecCtx, m_videoInputStream->codecpar) < 0)
    {
        qDebug() << "无法复制流参数" ;
        avcodec_free_context(&m_decodecCtx);
        m_decodecCtx=nullptr;
        avformat_free_context(m_inputFmtCtx);
        m_inputFmtCtx = nullptr;
        return false;
    }

    //打开解码器
    if (avcodec_open2(m_decodecCtx, codec, nullptr) < 0) {
       qDebug()<< "无法打开解码器" ;
        avcodec_free_context(&m_decodecCtx);
        m_decodecCtx=nullptr;
        avformat_free_context(m_inputFmtCtx);
        m_inputFmtCtx = nullptr;
        return false;
    }

    //初始化
    m_decodeSwsCtx = sws_getContext(
                                   m_decodecCtx->width, m_decodecCtx->height, m_decodecCtx->pix_fmt,
                                   m_width, m_height,  AV_PIX_FMT_RGB24,
                                   SWS_BILINEAR,
                                   nullptr, nullptr, nullptr);
    if(!m_decodeSwsCtx)
    {
        qDebug()<< "无法初始化图像转换器" ;
        avcodec_free_context(&m_decodecCtx);
        m_decodecCtx=nullptr;
        avformat_free_context(m_inputFmtCtx);
        m_inputFmtCtx = nullptr;
        return false;
    }
    m_isDeviceOpen = true;
    return true;

}
bool FFmpegVideoProcessor::decodeFrame(AVFrame*& yuvFrame)
{
    //读取码流包
    if (av_read_frame(m_inputFmtCtx, m_decode_pkt) < 0) {
        av_packet_unref(m_decode_pkt);
        return false;
    }

    //仅处理视频流包
    if (m_decode_pkt->stream_index != m_videoStreamIndex) {
        av_packet_unref(m_decode_pkt);
        return true; // 继续读取下一包
    }

    //发送包到解码器
    if (avcodec_send_packet(m_decodecCtx, m_decode_pkt) < 0) {
        av_packet_unref(m_decode_pkt);
        return false;
    }

    //解码获取YUV帧
    int ret = avcodec_receive_frame(m_decodecCtx, yuvFrame);
    av_packet_unref(m_decode_pkt);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return true;
    } else if (ret < 0) {
        qDebug() << "解码失败：" << ret;
        return false;
    }

    return true;

}
bool FFmpegVideoProcessor::readAndDecodeFrame(AVFrame*& yuvFrame)
{

    if (!m_isDeviceOpen || !m_decodecCtx) return false;
    if(!yuvFrame)
    {
        qDebug()<<"frame is null";
        yuvFrame = av_frame_alloc();
        av_frame_get_buffer(yuvFrame, 0);
    }
    int ret = 0;

    m_decode_pkt = av_packet_alloc();
    if(!m_decode_pkt)
    {
        qDebug()<<"pkt申请内存失败";
        return false;
    }
    while (true)
    {
        ret = av_read_frame(m_inputFmtCtx, m_decode_pkt);
        if (ret < 0)
        {
            qDebug()<<"av_read_frame failed";
            break;
        }
        if (m_decode_pkt->stream_index == m_videoStreamIndex)
        {
            // 发送数据包到解码器
            ret = avcodec_send_packet(m_decodecCtx, m_decode_pkt);
            if (ret < 0)
            {
                qDebug()<<"avcodec_send_packet failed";
               break;
            }
            // 接收解码帧
            ret = avcodec_receive_frame(m_decodecCtx, yuvFrame);
            if (ret == AVERROR(EAGAIN)) continue; // 需要更多数据
            if(ret==0)
            {
                av_packet_unref(m_decode_pkt);
                return true; // 成功获取帧
            }
            else
             {
                qDebug()<<"接收解码帧失败:"<<ret;
            }

        }

    }
    av_packet_unref(m_decode_pkt);
    return false;
}
bool FFmpegVideoProcessor::convertToRGB(const AVFrame* yuvFrame,int rgbWidth, int rgbHeight,uint8_t* rgbBuffer,AVFrame*& rgbFrame)
{
    if (!yuvFrame ||!m_decodeSwsCtx||!rgbBuffer||!rgbFrame) return false;

    // 初始化RGB帧结构
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize,
                        rgbBuffer, AV_PIX_FMT_RGB24,
                        rgbWidth, rgbHeight, 1);

    int ret = sws_scale(m_decodeSwsCtx,
                      yuvFrame->data, yuvFrame->linesize,
                        0, yuvFrame->height,
                        rgbFrame->data, rgbFrame->linesize);
    if (ret <= 0)
    {
        qDebug() << "格式转换失败，错误码：" << ret;
    }
    return true;
}
// 清理编码器资源
void FFmpegVideoProcessor::cleanupEncoder()
{
       //释放SwsContext
        if (m_encodeSwsCtx)
        {
            sws_freeContext(m_encodeSwsCtx);
            m_encodeSwsCtx = nullptr;
        }

        // 释放编码器上下文
        if (m_encodecCtx)
        {
            avcodec_free_context(&m_encodecCtx);
            m_encodecCtx = nullptr;
        }

        // 关闭输出文件IO
        if (m_outputFmtCtx) {
            if (!(m_outputFmtCtx->oformat->flags & AVFMT_NOFILE))
            {
                avio_closep(&m_outputFmtCtx->pb);
            }
            avformat_free_context(m_outputFmtCtx);
            m_outputFmtCtx = nullptr;
        }
        m_videoOutputStream = nullptr;
        if(m_encode_pkt)
        {
            av_packet_free(&m_encode_pkt);
            m_encode_pkt=nullptr;
        }
 }

void FFmpegVideoProcessor::cleanupDecoder()
{
    //释放SwsContext
     if (m_decodeSwsCtx)
     {
         sws_freeContext(m_decodeSwsCtx);
         m_decodeSwsCtx = nullptr;
     }

     // 释放解码器上下文
     if (m_decodecCtx)
     {
         avcodec_free_context(&m_decodecCtx);
         m_decodecCtx = nullptr;
     }
     // 关闭输入文件IO
     if (m_inputFmtCtx)
     {
         if (!(m_inputFmtCtx->iformat->flags & AVFMT_NOFILE))
         {
             avio_closep(&m_inputFmtCtx->pb);
         }

         avformat_free_context(m_inputFmtCtx);
         m_inputFmtCtx = nullptr;
     }

     m_videoInputStream = nullptr;
     if(m_decode_pkt)
     {
         av_packet_free(&m_decode_pkt);
         m_decode_pkt=nullptr;
     }
}
// 冲刷编码器（获取剩余数据）
void FFmpegVideoProcessor::flush_encoder(std::ofstream& output_file)
{
    int ret = avcodec_send_frame(m_encodecCtx, nullptr);
    if (ret < 0 && ret != AVERROR_EOF) {
       qDebug() << "冲刷编码器失败: " <<ret;
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(m_encodecCtx, m_encode_pkt);
        if (ret == AVERROR_EOF||ret==AVERROR(EAGAIN)) break;
        if (ret < 0) {
            qDebug() << "接收冲刷包失败: " <<ret;
            return;
        }
        output_file.write(reinterpret_cast<char*>(m_encode_pkt->data), m_encode_pkt->size);
        av_packet_unref(m_encode_pkt);
    }
    first_pts=0;


}
int FFmpegVideoProcessor::getOriginalWidth() const
{
    return m_decodecCtx ? m_decodecCtx->width : 0;
}
int FFmpegVideoProcessor::getOriginalHeight() const
{
    return m_decodecCtx ? m_decodecCtx->height : 0;
}
bool FFmpegVideoProcessor::seekToSec(double sec)
{
    // 1. 将秒数转换为时间戳（基于视频流的time_base）
    int64_t targetTs = av_rescale_q(static_cast<int64_t>(sec * AV_TIME_BASE),
                                   av_get_time_base_q(),
                                   m_videoInputStream->time_base);

    // 2. 执行seek（AVSEEK_FLAG_BACKWARD：找最近的关键帧）
    int ret=av_seek_frame(m_inputFmtCtx, m_videoStreamIndex, targetTs, AVSEEK_FLAG_BACKWARD);
    if ( ret< 0)
    {
        qDebug() << "Seek失败:"<<ret;
        return false;
    }

    // 清空解码器缓冲区
    avcodec_flush_buffers(m_decodecCtx);
    return true;
}
