#ifndef __H_FFMPEFG_DECODE_H__
#define __H_FFMPEFG_DECODE_H__

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#define INBUF_SIZE 4096

typedef void (*decoded_video_callback)(AVCodecContext* pContex, AVFrame* avFrame, int frameIndex, void* pParam);
typedef void (*decoded_audio_callback)(AVCodecContext* pContex, AVFrame* avFrame, int bufsize, uint8_t* buffer, void* pParam);

class Video_decode {
public:
	Video_decode(decoded_video_callback cb_fun_video=NULL, decoded_audio_callback cb_fun_audio = NULL, void*pContext=NULL);
	~Video_decode();
private:
	AVFormatContext* m_pFormatCtx;
	int m_videoStream;
	int m_audioStream;
	AVCodecContext* m_pVideoCodecCtx;
	AVCodecContext* m_pAudioCodecCtx;

	decoded_video_callback callback_video_fun;  // callback function
	decoded_audio_callback callback_audio_fun;
	void* m_pCallContext;
	bool m_bExit;
	bool m_bPause;

public:
	// int video_decode2(const char* filename);
	// void decode_init(const char* filename); // deprecated
	bool init_decode(const char* filename);
	int start_decode();
	void stop_decode();
	void pause_decode();
	bool paused();

public:
	const AVCodecContext* get_decode_context(bool bVideo = true);
private:
	void release_decode();
	AVCodecContext* open_codec_contex(int streamId);
	int video_decode(int frameId, AVCodecContext* pContex, AVFrame* pFrame, AVFrame* pFrameRGB, AVPacket* pPacket);
	int audio_decode(AVCodecContext* pContex, AVPacket* pkt, AVFrame* frame, struct SwrContext* swrCtx);
};
#endif
