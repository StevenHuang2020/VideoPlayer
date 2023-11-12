/**
 * @file video_state.h
 * @author Steven Huang
 */

#pragma once

#include "packets_sync.h"

class VideoStateData
{
public:
    explicit VideoStateData(bool use_hardware = false, bool loop_play = false);
    virtual ~VideoStateData();

public:
    bool has_video() const;
    bool has_audio() const;
    bool has_subtitle() const;
    AVCodecContext* get_contex(AVMediaType type) const;
    bool is_hardware_decode() const;
    int create_video_state(const char* filename);
    void delete_video_state();
    VideoState* get_state() const;
    void print_state() const;
    void threads_setting(VideoState* is, const Threads& threads);

private:
    VideoState* stream_open(const char* filename, const AVInputFormat* iformat = NULL);
    void stream_close(VideoState* is);
    int stream_component_open(VideoState* is, int stream_index);
    void stream_component_close(VideoState* is, int stream_index);
    int open_media(VideoState* is);
    enum AVHWDeviceType get_hwdevice(const char* device) const;
    enum AVPixelFormat get_hwdevice_decoder(const AVCodec* decoder, enum AVHWDeviceType type) const;
    bool open_hardware(AVCodecContext* avctx, const AVCodec* codec, const char* device = "dxva2");
    void close_hardware();
    int hw_decoder_init(AVCodecContext* ctx, const enum AVHWDeviceType type);
    void read_thread_exit_wait(VideoState* is);
    void threads_exit_wait(VideoState* is);

private:
    VideoState* m_pState{nullptr};

    bool m_bHasVideo{false};
    bool m_bHasAudio{false};
    bool m_bHasSubtitle{false};

    AVCodecContext* m_avctxVideo{nullptr};
    AVCodecContext* m_avctxAudio{nullptr};
    AVCodecContext* m_avctxSubtitle{nullptr};

    /**hardware decode**/
    bool m_bUseHardware{false};
    bool m_bHardwareSuccess{false};
    AVBufferRef* m_hw_device_ctx{nullptr};
    //enum AVPixelFormat m_hw_pix_fmt;
    bool m_bLoopPlay{false};
};
