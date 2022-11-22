/**
 * @file packets_sync.h
 * @author Steven Huang
 */

#ifndef __PACKETS_SYNC_H__
#define __PACKETS_SYNC_H__
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

 //only need to open audio filter, video will be synced
#define USE_AVFILTER_AUDIO	1
#define USE_AVFILTER_VIDEO	0

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/fifo.h>
#include <libavutil/time.h>
#include <libavutil/bprint.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>

#if USE_AVFILTER_AUDIO
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/avstring.h>
#include <libavutil/macros.h>
#endif
}

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 25
#define EXTERNAL_CLOCK_MIN_FRAMES 2
#define EXTERNAL_CLOCK_MAX_FRAMES 10

#define SDL_MIX_MAXVOLUME  128
/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

/* Step size for volume control in dB */
#define SDL_VOLUME_STEP (0.75)

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/* external clock speed adjustment constants for realtime sources based on buffer fullness */
#define EXTERNAL_CLOCK_SPEED_MIN  0.900
#define EXTERNAL_CLOCK_SPEED_MAX  1.010
#define EXTERNAL_CLOCK_SPEED_STEP 0.001

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

/* polls for possible required screen refresh at least this often, should be less than 1/fps */
#define REFRESH_RATE 0.01

/* NOTE: the size must be big enough to compensate the hardware audio buffersize size */
/* TODO: We assume that a decoded and resampled frame fits into this buffer */
#define SAMPLE_ARRAY_SIZE (8 * 65536)

#define CURSOR_HIDE_DELAY 1000000

#define USE_ONEPASS_SUBTITLE_RENDER 1


typedef struct MyAVPacketList {
	AVPacket* pkt;
	int serial;
} MyAVPacketList;

typedef struct PacketQueue {
	//AVFifoBuffer* pkt_list;
	AVFifo* pkt_list;
	int nb_packets;
	int size;
	int64_t duration;
	int abort_request;
	int serial;
	QMutex* mutex;
	QWaitCondition* cond;
} PacketQueue;

#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 20
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))


typedef struct AudioParams {
	int freq;
	int channels;
	AVChannelLayout channel_layout;
	enum AVSampleFormat fmt;
	int frame_size;
	int bytes_per_sec;
} AudioParams;

typedef struct Clock {
	double pts;           /* clock base */
	double pts_drift;     /* clock base minus time at which we updated the clock */
	double last_updated;
	double speed;
	int serial;           /* clock is based on a packet with this serial */
	int paused;
	int* queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
} Clock;

/* Common struct for handling all types of decoded data and allocated render buffers. */
typedef struct Frame {
	AVFrame* frame;
	AVSubtitle sub;
	int serial;
	double pts;           /* presentation timestamp for the frame */
	double duration;      /* estimated duration of the frame */
	int64_t pos;          /* byte position of the frame in the input file */
	int width;
	int height;
	int format;
	AVRational sar;
	int uploaded;
	int flip_v;
} Frame;

typedef struct FrameQueue {
	Frame queue[FRAME_QUEUE_SIZE];	// array queue model, loop queue
	int rindex;	// read pointer
	int windex;	// write pointer
	int size;	// current frame num
	int max_size;	// max frame num
	int keep_last;	// keep last frame
	int rindex_shown;	//current frame is shown
	QMutex* mutex;
	QWaitCondition* cond;
	PacketQueue* pktq;
} FrameQueue;

enum {
	AV_SYNC_AUDIO_MASTER, /* default choice */
	AV_SYNC_VIDEO_MASTER,
	AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};

typedef struct Decoder {
	AVPacket* pkt;
	PacketQueue* queue;
	AVCodecContext* avctx;
	int pkt_serial;
	int finished;
	int packet_pending;
	QWaitCondition* empty_queue_cond; // SDL_cond* empty_queue_cond;
	int64_t start_pts;
	AVRational start_pts_tb;
	int64_t next_pts;
	AVRational next_pts_tb;
	// SDL_Thread* decoder_tid;
	void* decoder_tid; //thread pointer
	char* decoder_name;
} Decoder;

typedef struct Threads {
	QThread* read_tid;
	QThread* video_decode_tid;
	QThread* audio_decode_tid;
	QThread* video_play_tid;
	QThread* audio_play_tid;
	QThread* subtitle_decode_tid;
}Threads;

typedef struct VideoState {
	const AVInputFormat* iformat;
	int abort_request;
	int force_refresh;
	int paused;
	int last_paused;
	int queue_attachments_req;
	int seek_req;
	int seek_flags;
	int64_t seek_pos;
	int64_t seek_rel;
	int read_pause_return;
	AVFormatContext* ic;
	int realtime;

	Clock vidclk;
	Clock audclk;
	Clock extclk;

	PacketQueue videoq;
	PacketQueue audioq;
	PacketQueue subtitleq;

	FrameQueue pictq;
	FrameQueue sampq;
	FrameQueue subpq;

	Decoder viddec;
	Decoder auddec;
	Decoder subdec;

	int audio_stream;
	int av_sync_type;

	double audio_clock;
	int audio_clock_serial;

	double audio_clock_old; //add for renew clock after speed changing

	AVStream* audio_st;

	int audio_volume;
	int muted;

	int frame_drops_early;
	int frame_drops_late;

#if 0
	double audio_diff_avg_coef;
	int audio_diff_avg_count;
	double audio_diff_cum; /* used for AV difference average computation */
	double audio_diff_threshold;

	int16_t sample_array[SAMPLE_ARRAY_SIZE];
	int sample_array_index;

	uint8_t* audio_buf;
	uint8_t* audio_buf1;
	unsigned int audio_buf_size; /* in bytes */
	unsigned int audio_buf1_size;
	int audio_buf_index; /* in bytes */
	int audio_write_buf_size;

	int audio_hw_buf_size;

	struct SwrContext* swr_ctx;

	enum ShowMode {
		SHOW_MODE_NONE = -1, SHOW_MODE_VIDEO = 0, SHOW_MODE_WAVES, SHOW_MODE_RDFT, SHOW_MODE_NB
	} show_mode;
#endif

	int last_i_start;
	//RDFTContext* rdft;
	int rdft_bits;
	//FFTSample* rdft_data;
	int xpos;
	double last_vis_time;
	//SDL_Texture* vis_texture;
	//SDL_Texture* sub_texture;
	//SDL_Texture* vid_texture;

	int subtitle_stream;
	AVStream* subtitle_st;

	double frame_timer;
	double frame_last_returned_time;
	double frame_last_filter_delay;
	int video_stream;
	AVStream* video_st;

	double max_frame_duration;      // maximum duration of a frame - above this, we consider the jump a timestamp discontinuity
	struct SwsContext* img_convert_ctx;
	struct SwsContext* sub_convert_ctx;
	int eof;
	int loop;

	char* filename;
	int width, height, xleft, ytop;
	int step;

#if USE_AVFILTER_AUDIO
	struct AudioParams audio_src;
	struct AudioParams audio_tgt;

	double audio_speed;
	char* afilters;
	int req_afilter_reconfigure;
	char* vfilters;
	int req_vfilter_reconfigure;

	struct AudioParams audio_filter_src;
	int vfilter_idx;
	AVFilterContext* in_video_filter;   // the first filter in the video chain
	AVFilterContext* out_video_filter;  // the last filter in the video chain
	AVFilterContext* in_audio_filter;   // the first filter in the audio chain
	AVFilterContext* out_audio_filter;  // the last filter in the audio chain
	AVFilterGraph* agraph;
	AVFilterGraph* vgraph;
#endif

	int last_video_stream, last_audio_stream, last_subtitle_stream;

	QWaitCondition* continue_read_thread;
	// int read_thread_exit;
	// void* read_tid; //read thread pointer

	Threads threads; //all thread would access VideoState
} VideoState;


#if !NDEBUG
#define PRINT_PACKETQUEUE_INFO  0
#define PRINT_PACKETQUEUE_AUDIO_INFO  0
#else
#define PRINT_PACKETQUEUE_INFO  0
#define PRINT_PACKETQUEUE_AUDIO_INFO  0
#endif

/***************PacketQueue operations*****************/
int packet_queue_init(PacketQueue* q);
void packet_queue_destroy(PacketQueue* q);
void packet_queue_flush(PacketQueue* q);
void packet_queue_start(PacketQueue* q);
void packet_queue_abort(PacketQueue* q);
int packet_queue_get(PacketQueue* q, AVPacket* pkt, int block, int* serial);
int packet_queue_put(PacketQueue* q, AVPacket* pkt);
int packet_queue_put_nullpacket(PacketQueue* q, AVPacket* pkt, int stream_index);
int packet_queue_put_private(PacketQueue* q, AVPacket* pkt);
void packet_queue_print(const PacketQueue* q, const AVPacket* pkt, const QString& prefix);


/***************FrameQueue operations*****************/
int frame_queue_init(FrameQueue* f, PacketQueue* pktq, int max_size, int keep_last);
void frame_queue_destory(FrameQueue* f);
void frame_queue_unref_item(Frame* vp);
void frame_queue_signal(FrameQueue* f);
Frame* frame_queue_peek_writable(FrameQueue* f);
Frame* frame_queue_peek(FrameQueue* f);
Frame* frame_queue_peek_next(FrameQueue* f);
Frame* frame_queue_peek_last(FrameQueue* f);
Frame* frame_queue_peek_readable(FrameQueue* f);
void frame_queue_push(FrameQueue* f);
void frame_queue_next(FrameQueue* f);
int frame_queue_nb_remaining(FrameQueue* f);
int64_t frame_queue_last_pos(FrameQueue* f);

int queue_picture(VideoState* is, AVFrame* src_frame, double pts, double duration, int64_t pos, int serial);
int get_video_frame(VideoState* is, AVFrame* frame);


/***************Decoder operations*****************/
int decoder_init(Decoder* d, AVCodecContext* avctx, PacketQueue* queue, QWaitCondition* empty_queue_cond);
int decoder_decode_frame(Decoder* d, AVFrame* frame, AVSubtitle* sub);
void decoder_destroy(Decoder* d);
int decoder_start(Decoder* d, void* thread, const char* thread_name);
void decoder_abort(Decoder* d, FrameQueue* fq);
void get_file_info(const char* filename, int64_t& duration);
void get_duration_time(const int64_t duration_us, int64_t& hours, int64_t& mins, int64_t& secs, int64_t& us);

/***************Clock operations*****************/
double get_clock(Clock* c);
void set_clock_at(Clock* c, double pts, int serial, double time);
void set_clock(Clock* c, double pts, int serial);
void set_clock_speed(Clock* c, double speed);
void init_clock(Clock* c, int* queue_serial);
void sync_clock_to_slave(Clock* c, Clock* slave);

/***************VideoState operations*****************/
int get_master_sync_type(VideoState* is);
double get_master_clock(VideoState* is);
void check_external_clock_speed(VideoState* is);
void stream_seek(VideoState* is, int64_t pos, int64_t rel, int seek_by_bytes);
//void stream_toggle_pause(VideoState* is, bool pause = true);
void toggle_pause(VideoState* is, bool pause = true);
void toggle_mute(VideoState* is, bool mute = true);
void update_volume(VideoState* is, int sign, double step);
void step_to_next_frame(VideoState* is);
double compute_target_delay(double delay, VideoState* is);
double vp_duration(VideoState* is, Frame* vp, Frame* nextvp);
void update_video_pts(VideoState* is, double pts, int serial);

#if PRINT_PACKETQUEUE_INFO
void print_state_info(VideoState* is);
#endif

/****************************************/
int is_realtime(AVFormatContext* s);
int stream_has_enough_packets(AVStream* st, int stream_id, PacketQueue* queue);

#if USE_AVFILTER_AUDIO
void set_audio_playspeed(VideoState* is, double value);

int cmp_audio_fmts(enum AVSampleFormat fmt1, int64_t channel_count1,
	enum AVSampleFormat fmt2, int64_t channel_count2);
// int64_t get_valid_channel_layout(int64_t channel_layout, int channels);

int configure_audio_filters(VideoState* is, const char* afilters, int force_output_format);
int configure_filtergraph(AVFilterGraph* graph, const char* filtergraph,
	AVFilterContext* source_ctx, AVFilterContext* sink_ctx);

//int audio_open(void* opaque, int64_t wanted_channel_layout, int wanted_nb_channels, int wanted_sample_rate, struct AudioParams* audio_hw_params);

void set_video_playspeed(VideoState* is);
int configure_video_filters(AVFilterGraph* graph, VideoState* is, const char* vfilters, AVFrame* frame);
#endif


#endif /* end of __PACKETS_SYNC_H__ */