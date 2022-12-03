/**
 * @file video_state.h
 * @author Steven Huang
 */

#ifndef VIDEO_STATE_H
#define VIDEO_STATE_H

#include "packets_sync.h"

class VideoStateData {
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
	VideoState* m_pState;

	bool m_bHasVideo;
	bool m_bHasAudio;
	bool m_bHasSubtitle;

	AVCodecContext* m_avctxVideo;
	AVCodecContext* m_avctxAudio;
	AVCodecContext* m_avctxSubtitle;

	/**hardware decode**/
	bool m_bUseHardware;
	bool m_bHardwareSuccess;
	AVBufferRef* m_hw_device_ctx;
	//enum AVPixelFormat m_hw_pix_fmt;
	bool m_bLoopPlay;
};
#endif /* end of VIDEO_STATE_H */