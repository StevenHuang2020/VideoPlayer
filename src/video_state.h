/**
 * @file video_state.h
 * @author Steven Huang
 */

#ifndef VIDEO_STATE_H
#define VIDEO_STATE_H

#include "packets_sync.h"

class VideoStateData {
public:
	VideoStateData(QThread* pThread, bool use_hardware = false);
	~VideoStateData();
private:
	VideoState* m_pState;

	QThread* m_pReadThreadId;

	bool m_bHasVideo;
	bool m_bHasAudio;
	bool m_bHasSubtitle;

	AVCodecContext* m_avctxVideo;
	AVCodecContext* m_avctxAudio;
	AVCodecContext* m_avctxSubtitle;

	/**hardware decode**/
	bool m_bUseHardware;
	bool m_bHardwareSuccess;
	//AVBufferRef* m_hw_device_ctx;
	//enum AVPixelFormat m_hw_pix_fmt;

private:
	VideoState* stream_open(const char* filename, const AVInputFormat* iformat = NULL);
	void stream_close(VideoState* is);
	int stream_component_open(VideoState* is, int stream_index);
	void stream_component_close(VideoState* is, int stream_index);
	int open_media(VideoState* is);

	enum AVHWDeviceType get_hwdevice(const char* device);
	enum AVPixelFormat get_hwdevice_decoder(const AVCodec* decoder, enum AVHWDeviceType type);
	bool open_hardware(AVCodecContext* avctx, const AVCodec* codec, const char* device = "dxva2");
	void close_hardware();
public:
	bool has_video();
	bool has_audio();
	bool has_subtitle();
	AVCodecContext* get_contex(AVMediaType type);
	bool is_hardware_decode();
public:
	int create_video_state(const char* filename);
	void delete_video_state();
	VideoState* get_state();
	void print_state();

};
#endif /* end of VIDEO_STATE_H */