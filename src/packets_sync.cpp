// ***********************************************************/
// packets_sync.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// packets A/V synchronization struct and operations definition.
// This code is referenced from ffplay.c in Ffmpeg library.
// ***********************************************************/

#include "packets_sync.h"

int framedrop = -1;
//static int decoder_reorder_pts = -1;
//static int display_disable = 1;
//static int64_t audio_callback_time;

int packet_queue_init(PacketQueue* q)
{
	memset(q, 0, sizeof(PacketQueue));
	q->pkt_list = av_fifo_alloc(sizeof(MyAVPacketList));
	if (!q->pkt_list)
		return AVERROR(ENOMEM);
	q->mutex = new QMutex();
	if (!q->mutex) {
		av_log(NULL, AV_LOG_FATAL, "new QMutex() error.\n");
		return AVERROR(ENOMEM);
	}
	q->cond = new QWaitCondition();
	if (!q->cond) {
		av_log(NULL, AV_LOG_FATAL, "new QWaitCondition() error.\n");
		return AVERROR(ENOMEM);
	}
	q->abort_request = 1;
	return 0;
}

void packet_queue_destroy(PacketQueue* q)
{
	packet_queue_flush(q);
	av_fifo_freep(&q->pkt_list);
	delete q->mutex;
	delete q->cond;
}

void packet_queue_flush(PacketQueue* q)
{
	MyAVPacketList pkt1;

	q->mutex->lock();
	while (av_fifo_size(q->pkt_list) >= sizeof(pkt1)) {
		av_fifo_generic_read(q->pkt_list, &pkt1, sizeof(pkt1), NULL);
		av_packet_free(&pkt1.pkt);
	}
	q->nb_packets = 0;
	q->size = 0;
	q->duration = 0;
	q->serial++;
	q->mutex->unlock();
}

void packet_queue_start(PacketQueue* q)
{
	q->mutex->lock();
	q->abort_request = 0;
	q->serial++;
	q->mutex->unlock();
}

void packet_queue_abort(PacketQueue* q)
{
	q->mutex->lock();
	q->abort_request = 1;
	q->cond->wakeAll();
	q->mutex->unlock();
}

int packet_queue_get(PacketQueue* q, AVPacket* pkt, int block, int* serial)
{
#if PRINT_PACKETQUEUE_INFO
	//packet_queue_print(q, pkt, "packet_queue_get");
#endif

	MyAVPacketList pkt1;
	int ret = 0;

	q->mutex->lock();

	for (;;) {
		if (q->abort_request) {
			ret = -1;
			break;
		}

		if (av_fifo_size(q->pkt_list) >= sizeof(pkt1)) {
			av_fifo_generic_read(q->pkt_list, &pkt1, sizeof(pkt1), NULL);
			q->nb_packets--;
			q->size -= pkt1.pkt->size + sizeof(pkt1);
			q->duration -= pkt1.pkt->duration;
			av_packet_move_ref(pkt, pkt1.pkt);
			if (serial)
				*serial = pkt1.serial;
			av_packet_free(&pkt1.pkt);
			ret = 1;
			break;
		}
		else if (!block) {
			ret = 0;
			break;
		}
		else {
			q->cond->wait(q->mutex);
		}
	}
	q->mutex->unlock();
	return ret;
}

void packet_queue_print(const PacketQueue* q, const AVPacket* pkt, const QString& prefix)
{
	qDebug("[%s]Queue:[%p](nb_packets:%d, size:%d, dur:%d, serial:%d), pkt(pts:%lld,dts:%lld,size:%d,s_index:%d,dur:%lld,pos:%lld).",
		qUtf8Printable(prefix), q, q->nb_packets, q->size, q->duration, q->serial,
		pkt->pts, pkt->dts, pkt->size, pkt->stream_index, pkt->duration, pkt->pos);
}

int packet_queue_put(PacketQueue* q, AVPacket* pkt)
{
	AVPacket* pkt1;
	int ret = -1;

	pkt1 = av_packet_alloc();
	if (!pkt1) {
		av_packet_unref(pkt);
		return ret;
	}
	av_packet_move_ref(pkt1, pkt);

	q->mutex->lock();
	ret = packet_queue_put_private(q, pkt1);
	q->mutex->unlock();

	if (ret < 0)
		av_packet_free(&pkt1);

#if PRINT_PACKETQUEUE_INFO
	//packet_queue_print(q, pkt, "packet_queue_put");
#endif
	return ret;
}

int packet_queue_put_nullpacket(PacketQueue* q, AVPacket* pkt, int stream_index)
{
	pkt->stream_index = stream_index;
	return packet_queue_put(q, pkt);
}

int packet_queue_put_private(PacketQueue* q, AVPacket* pkt)
{
	MyAVPacketList pkt1;

	if (q->abort_request)
		return -1;

	if (av_fifo_space(q->pkt_list) < sizeof(pkt1)) {
		if (av_fifo_grow(q->pkt_list, sizeof(pkt1)) < 0)
			return -1;
	}

	pkt1.pkt = pkt;
	pkt1.serial = q->serial;

	av_fifo_generic_write(q->pkt_list, &pkt1, sizeof(pkt1), NULL);
	q->nb_packets++;
	q->size += pkt1.pkt->size + sizeof(pkt1);
	q->duration += pkt1.pkt->duration;
	/* XXX: should duplicate packet data in DV case */
	q->cond->wakeAll();
	return 0;
}

int frame_queue_init(FrameQueue* f, PacketQueue* pktq, int max_size, int keep_last)
{
	int i;
	memset(f, 0, sizeof(FrameQueue));
	if (!(f->mutex = new QMutex())) {
		av_log(NULL, AV_LOG_FATAL, "new QMutex() error!\n");
		return AVERROR(ENOMEM);
	}
	if (!(f->cond = new QWaitCondition())) {
		av_log(NULL, AV_LOG_FATAL, "new QWaitCondition() error\n");
		return AVERROR(ENOMEM);
	}
	f->pktq = pktq;
	f->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
	f->keep_last = !!keep_last;
	for (i = 0; i < f->max_size; i++)
		if (!(f->queue[i].frame = av_frame_alloc()))
			return AVERROR(ENOMEM);
	return 0;
}

void frame_queue_destory(FrameQueue* f)
{
	int i;
	for (i = 0; i < f->max_size; i++) {
		Frame* vp = &f->queue[i];
		frame_queue_unref_item(vp);
		av_frame_free(&vp->frame);
	}

	delete f->mutex;
	delete f->cond;
}

void frame_queue_unref_item(Frame* vp)
{
	av_frame_unref(vp->frame);//frame reference number reduce 1
	avsubtitle_free(&vp->sub);//sub
}

void frame_queue_signal(FrameQueue* f)
{
	f->mutex->lock();
	f->cond->wakeAll();
	f->mutex->unlock();
}

Frame* frame_queue_peek(FrameQueue* f)
{
	return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}

Frame* frame_queue_peek_next(FrameQueue* f)
{
	return &f->queue[(f->rindex + f->rindex_shown + 1) % f->max_size];
}

Frame* frame_queue_peek_last(FrameQueue* f)
{
	return &f->queue[f->rindex];
}

Frame* frame_queue_peek_writable(FrameQueue* f)
{
	/* wait until we have space to put a new frame */
	f->mutex->lock();
	while (f->size >= f->max_size &&
		!f->pktq->abort_request) {
		f->cond->wait(f->mutex);
	}
	f->mutex->unlock();

	if (f->pktq->abort_request)
		return NULL;

	return &f->queue[f->windex];
}

Frame* frame_queue_peek_readable(FrameQueue* f)
{
	/* wait until we have a readable a new frame */
	f->mutex->lock();
	while (f->size - f->rindex_shown <= 0 &&
		!f->pktq->abort_request) {
		f->cond->wait(f->mutex);
	}
	f->mutex->unlock();

	if (f->pktq->abort_request)
		return NULL;

	return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}

void frame_queue_push(FrameQueue* f)
{
	if (++f->windex == f->max_size)
		f->windex = 0;
	f->mutex->lock();
	f->size++;
	f->cond->wakeAll();
	f->mutex->unlock();
}

void frame_queue_next(FrameQueue* f)
{
	if (f->keep_last && !f->rindex_shown) {
		f->rindex_shown = 1;
		return;
	}
	frame_queue_unref_item(&f->queue[f->rindex]);
	if (++f->rindex == f->max_size)
		f->rindex = 0;
	f->mutex->lock();
	f->size--;
	f->cond->wakeAll();
	f->mutex->unlock();
}

/* return the number of undisplayed frames in the queue */
int frame_queue_nb_remaining(FrameQueue* f)
{
	return f->size - f->rindex_shown;
}

/* return last shown position */
int64_t frame_queue_last_pos(FrameQueue* f)
{
	Frame* fp = &f->queue[f->rindex];
	if (f->rindex_shown && fp->serial == f->pktq->serial)
		return fp->pos;
	else
		return -1;
}

int queue_picture(VideoState* is, AVFrame* src_frame, double pts, double duration, int64_t pos, int serial)
{
#if PRINT_PACKETQUEUE_INFO
	//int64 lld, double lf
	qDebug("queue picture, w:%d, h:%d, nb:%d, ft:%d(%s), kf:%d, pic_t:%d(%c), pts:%lf, duration:%lf, pos:%lld, serial:%d",
		src_frame->width, src_frame->height, src_frame->nb_samples, src_frame->format,
		av_get_sample_fmt_name(AVSampleFormat(src_frame->format)),
		src_frame->key_frame, src_frame->pict_type, av_get_picture_type_char(src_frame->pict_type),
		pts, duration, pos, serial);
#endif

	Frame* vp;

	if (!(vp = frame_queue_peek_writable(&is->pictq)))
		return -1;

	vp->sar = src_frame->sample_aspect_ratio;
	vp->uploaded = 0;

	vp->width = src_frame->width;
	vp->height = src_frame->height;
	vp->format = src_frame->format;

	vp->pts = pts;
	vp->duration = duration;
	vp->pos = pos;
	vp->serial = serial;

	av_frame_move_ref(vp->frame, src_frame);
	frame_queue_push(&is->pictq);
	return 0;
}

int get_video_frame(VideoState* is, AVFrame* frame)
{
	int got_picture = -1;

	if ((got_picture = decoder_decode_frame(&is->viddec, frame, NULL)) < 0)
		return -1;

	if (got_picture) {
		double dpts = NAN;

		if (frame->pts != AV_NOPTS_VALUE)
			dpts = av_q2d(is->video_st->time_base) * frame->pts;

		frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(is->ic, is->video_st, frame);
#if 0/**drop frame**/
		if (framedrop > 0 || (framedrop && get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER)) {
			if (frame->pts != AV_NOPTS_VALUE) {
				double diff = dpts - get_master_clock(is);
				if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD &&
					diff - is->frame_last_filter_delay < 0 &&
					is->viddec.pkt_serial == is->vidclk.serial &&
					is->videoq.nb_packets) {
					is->frame_drops_early++;
					av_frame_unref(frame);
					got_picture = 0;
				}
			}
		}
#endif
	}

	return got_picture;
}

int decoder_init(Decoder* d, AVCodecContext* avctx, PacketQueue* queue, QWaitCondition* empty_queue_cond) {
	memset(d, 0, sizeof(Decoder));
	d->pkt = av_packet_alloc();
	if (!d->pkt)
		return AVERROR(ENOMEM);
	d->avctx = avctx;
	d->queue = queue;
	d->empty_queue_cond = empty_queue_cond;
	d->start_pts = AV_NOPTS_VALUE;
	d->pkt_serial = -1;
	return 0;
}

int decoder_start(Decoder* d, void* thread, const char* thread_name, void* arg)
{
	packet_queue_start(d->queue);
	d->decoder_tid = thread;
	d->decoder_name = av_strdup(thread_name);
	return 0;
}

void decoder_destroy(Decoder* d) {
	av_packet_free(&d->pkt);
	avcodec_free_context(&d->avctx);
	av_free(d->decoder_name);
}

void decoder_abort(Decoder* d, FrameQueue* fq)
{
	packet_queue_abort(d->queue);
	frame_queue_signal(fq);
	//SDL_WaitThread(d->decoder_tid, NULL);
	((QThread*)(d->decoder_tid))->wait();
	d->decoder_tid = NULL;
	packet_queue_flush(d->queue);
}

int decoder_decode_frame(Decoder* d, AVFrame* frame, AVSubtitle* sub) {
	int ret = AVERROR(EAGAIN);
	int decoder_reorder_pts = -1;
	for (;;) {
		if (d->queue->serial == d->pkt_serial) {
			do {
				if (d->queue->abort_request)
					return -1;

				switch (d->avctx->codec_type) {
				case AVMEDIA_TYPE_VIDEO:
					ret = avcodec_receive_frame(d->avctx, frame);
					if (ret >= 0) {
						if (decoder_reorder_pts == -1) {
							frame->pts = frame->best_effort_timestamp;
						}
						else if (!decoder_reorder_pts) {
							frame->pts = frame->pkt_dts;
						}
					}
					break;
				case AVMEDIA_TYPE_AUDIO:
					ret = avcodec_receive_frame(d->avctx, frame);
					if (ret >= 0) {
						AVRational tb = { 1, frame->sample_rate };
						if (frame->pts != AV_NOPTS_VALUE)
							frame->pts = av_rescale_q(frame->pts, d->avctx->pkt_timebase, tb);
						else if (d->next_pts != AV_NOPTS_VALUE)
							frame->pts = av_rescale_q(d->next_pts, d->next_pts_tb, tb);
						if (frame->pts != AV_NOPTS_VALUE) {
							d->next_pts = frame->pts + frame->nb_samples;
							d->next_pts_tb = tb;
						}
					}
					break;
				}
				if (ret == AVERROR_EOF) {
					d->finished = d->pkt_serial;
					avcodec_flush_buffers(d->avctx);
					return 0;
				}
				if (ret >= 0)
					return 1;
			} while (ret != AVERROR(EAGAIN));
		}

		do {
			if (d->queue->nb_packets == 0)
				//SDL_CondSignal(d->empty_queue_cond);
				d->empty_queue_cond->wakeAll();

			if (d->packet_pending) {
				d->packet_pending = 0;
			}
			else {
				int old_serial = d->pkt_serial;
				if (packet_queue_get(d->queue, d->pkt, 1, &d->pkt_serial) < 0)
					return -1;
				if (old_serial != d->pkt_serial) {
					avcodec_flush_buffers(d->avctx);
					d->finished = 0;
					d->next_pts = d->start_pts;
					d->next_pts_tb = d->start_pts_tb;
				}
			}
			if (d->queue->serial == d->pkt_serial)
				break;
			av_packet_unref(d->pkt);
		} while (1);

		if (d->avctx->codec_type == AVMEDIA_TYPE_SUBTITLE) {
			int got_frame = 0;
			ret = avcodec_decode_subtitle2(d->avctx, sub, &got_frame, d->pkt);
			if (ret < 0) {
				ret = AVERROR(EAGAIN);
			}
			else {
				if (got_frame && !d->pkt->data) {
					d->packet_pending = 1;
				}
				ret = got_frame ? 0 : (d->pkt->data ? AVERROR(EAGAIN) : AVERROR_EOF);
			}
			av_packet_unref(d->pkt);
		}
		else {
			if (avcodec_send_packet(d->avctx, d->pkt) == AVERROR(EAGAIN)) {
				av_log(d->avctx, AV_LOG_ERROR, "Receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
				d->packet_pending = 1;
			}
			else {
				av_packet_unref(d->pkt);
			}
		}
	}
}

double get_clock(Clock* c)
{
	if (*c->queue_serial != c->serial)
		return NAN;
	if (c->paused) {
		return c->pts;
	}
	else {
		double time = av_gettime_relative() / 1000000.0;
		return c->pts_drift + time - (time - c->last_updated) * (1.0 - c->speed);
	}
}

void set_clock_at(Clock* c, double pts, int serial, double time)
{
	c->pts = pts;
	c->last_updated = time;
	c->pts_drift = c->pts - time;
	c->serial = serial;
}

void set_clock(Clock* c, double pts, int serial)
{
	double time = av_gettime_relative() / 1000000.0;
	set_clock_at(c, pts, serial, time);
}

void set_clock_speed(Clock* c, double speed)
{
	set_clock(c, get_clock(c), c->serial);
	c->speed = speed;
}

void init_clock(Clock* c, int* queue_serial)
{
	c->speed = 1.0;
	c->paused = 0;
	c->queue_serial = queue_serial;
	set_clock(c, NAN, -1);
}

void sync_clock_to_slave(Clock* c, Clock* slave)
{
	double clock = get_clock(c);
	double slave_clock = get_clock(slave);
	if (!isnan(slave_clock) && (isnan(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD))
		set_clock(c, slave_clock, slave->serial);
}

int stream_has_enough_packets(AVStream* st, int stream_id, PacketQueue* queue) {
	return stream_id < 0 ||
		queue->abort_request ||
		(st->disposition & AV_DISPOSITION_ATTACHED_PIC) ||
		queue->nb_packets > MIN_FRAMES && (!queue->duration || av_q2d(st->time_base) * queue->duration > 1.0);
}

int is_realtime(AVFormatContext* s)
{
	if (!strcmp(s->iformat->name, "rtp")
		|| !strcmp(s->iformat->name, "rtsp")
		|| !strcmp(s->iformat->name, "sdp")
		)
		return 1;

	if (s->pb && (!strncmp(s->url, "rtp:", 4)
		|| !strncmp(s->url, "udp:", 4)
		)
		)
		return 1;
	return 0;
}

int get_master_sync_type(VideoState* is)
{
	if (is->av_sync_type == AV_SYNC_VIDEO_MASTER) {
		if (is->video_st)
			return AV_SYNC_VIDEO_MASTER;
		else
			return AV_SYNC_AUDIO_MASTER;
	}
	else if (is->av_sync_type == AV_SYNC_AUDIO_MASTER) {
		if (is->audio_st)
			return AV_SYNC_AUDIO_MASTER;
		else
			return AV_SYNC_EXTERNAL_CLOCK;
	}
	else {
		return AV_SYNC_EXTERNAL_CLOCK;
	}
}

/* get the current master clock value */
double get_master_clock(VideoState* is)
{
	double val;

	switch (get_master_sync_type(is)) {
	case AV_SYNC_VIDEO_MASTER:
		val = get_clock(&is->vidclk);
		break;
	case AV_SYNC_AUDIO_MASTER:
		val = get_clock(&is->audclk);
		break;
	default:
		val = get_clock(&is->extclk);
		break;
	}
	return val;
}

void check_external_clock_speed(VideoState* is)
{
	if (is->video_stream >= 0 && is->videoq.nb_packets <= EXTERNAL_CLOCK_MIN_FRAMES ||
		is->audio_stream >= 0 && is->audioq.nb_packets <= EXTERNAL_CLOCK_MIN_FRAMES) {
		set_clock_speed(&is->extclk, FFMAX(EXTERNAL_CLOCK_SPEED_MIN, is->extclk.speed - EXTERNAL_CLOCK_SPEED_STEP));
	}
	else if ((is->video_stream < 0 || is->videoq.nb_packets > EXTERNAL_CLOCK_MAX_FRAMES) &&
		(is->audio_stream < 0 || is->audioq.nb_packets > EXTERNAL_CLOCK_MAX_FRAMES)) {
		set_clock_speed(&is->extclk, FFMIN(EXTERNAL_CLOCK_SPEED_MAX, is->extclk.speed + EXTERNAL_CLOCK_SPEED_STEP));
	}
	else {
		double speed = is->extclk.speed;
		if (speed != 1.0)
			set_clock_speed(&is->extclk, speed + EXTERNAL_CLOCK_SPEED_STEP * (1.0 - speed) / fabs(1.0 - speed));
	}
}

/* seek in the stream */
void stream_seek(VideoState* is, int64_t pos, int64_t rel, int seek_by_bytes)
{
	if (!is->seek_req) {
		is->seek_pos = pos;
		is->seek_rel = rel;
		is->seek_flags &= ~AVSEEK_FLAG_BYTE;
		if (seek_by_bytes)
			is->seek_flags |= AVSEEK_FLAG_BYTE;
		is->seek_req = 1;
		//SDL_CondSignal(is->continue_read_thread);
		is->continue_read_thread->wakeAll();
	}
}

/* pause or resume the video */
void stream_toggle_pause(VideoState* is)
{
	if (is->paused) {
		is->frame_timer += av_gettime_relative() / 1000000.0 - is->vidclk.last_updated;
		if (is->read_pause_return != AVERROR(ENOSYS)) {
			is->vidclk.paused = 0;
		}
		set_clock(&is->vidclk, get_clock(&is->vidclk), is->vidclk.serial);
	}
	set_clock(&is->extclk, get_clock(&is->extclk), is->extclk.serial);
	is->paused = is->audclk.paused = is->vidclk.paused = is->extclk.paused = !is->paused;
	is->step = 0;
}

void toggle_pause(VideoState* is)
{
	stream_toggle_pause(is);
}

void toggle_mute(VideoState* is, bool mute)
{
#if 1
	bool muted = !!is->muted;
	if (muted != mute ) {
		is->muted = mute;
	}
#else
	is->muted = !is->muted;
#endif
}

void update_volume(VideoState* is, int sign, double step)
{
	double volume_level = is->audio_volume ? (20 * log(is->audio_volume / (double)SDL_MIX_MAXVOLUME) / log(10)) : -1000.0;
	int new_volume = lrint(SDL_MIX_MAXVOLUME * pow(10.0, (volume_level + sign * step) / 20.0));
	is->audio_volume = av_clip(is->audio_volume == new_volume ? (is->audio_volume + sign) : new_volume, 0, SDL_MIX_MAXVOLUME);
}

void step_to_next_frame(VideoState* is)
{
	/* if the stream is paused unpause it, then step */
	if (is->paused)
		stream_toggle_pause(is);
	is->step = 1;
}

double compute_target_delay(double delay, VideoState* is)
{
	double sync_threshold, diff = 0;

	/* update delay to follow master synchronisation source */
	if (get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER) {
		/* if video is slave, we try to correct big delays by
		   duplicating or deleting a frame */
		diff = get_clock(&is->vidclk) - get_master_clock(is);

		/* skip or repeat frame. We take into account the
		   delay to compute the threshold. I still don't know
		   if it is the best guess */
		sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
		if (!isnan(diff) && fabs(diff) < is->max_frame_duration) {
			if (diff <= -sync_threshold)
				delay = FFMAX(0, delay + diff);
			else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
				delay = delay + diff;
			else if (diff >= sync_threshold)
				delay = 2 * delay;
		}
	}

	av_log(NULL, AV_LOG_TRACE, "video: delay=%0.3f A-V=%f\n",
		delay, -diff);

	return delay;
}

double vp_duration(VideoState* is, Frame* vp, Frame* nextvp) {
	if (vp->serial == nextvp->serial) {
		double duration = nextvp->pts - vp->pts;
		if (isnan(duration) || duration <= 0 || duration > is->max_frame_duration)
			return vp->duration;
		else
			return duration;
	}
	else {
		return 0.0;
	}
}

void update_video_pts(VideoState* is, double pts, int64_t pos, int serial) {
	/* update current video pts */
	set_clock(&is->vidclk, pts, serial);
	sync_clock_to_slave(&is->extclk, &is->vidclk);
}

void print_state_info(VideoState* is)
{
#if PRINT_PACKETQUEUE_INFO
	if (is) {
		PacketQueue* pPacket = &is->videoq;
		qDebug("[VideoState] V PacketQueue[%p](nb_packets:%d,size:%d,dur:%lld, abort:%d, serial:%d)",
			pPacket, pPacket->nb_packets, pPacket->size, pPacket->duration, pPacket->abort_request, pPacket->serial);

		pPacket = &is->audioq;
		qDebug("[VideoState] A PacketQueue[%p](nb_packets:%d,size:%d,dur:%lld, abort:%d, serial:%d)",
			pPacket, pPacket->nb_packets, pPacket->size, pPacket->duration, pPacket->abort_request, pPacket->serial);

		pPacket = &is->subtitleq;
		qDebug("[VideoState] S PacketQueue[%p](nb_packets:%d,size:%d,dur:%lld, abort:%d, serial:%d)",
			pPacket, pPacket->nb_packets, pPacket->size, pPacket->duration, pPacket->abort_request, pPacket->serial);

		/*qDebug("[VideoState]FrameQueue(v:%p,a:%p,s:%p)",
			&is->pictq, &is->sampq, &is->subpq);
		qDebug("[VideoState]Decoder(v:%p,a:%p,s:%p)",
			&is->viddec, &is->auddec, &is->subdec);
		qDebug("[VideoState]Clock(v:%p,a:%p,s:%p)",
			&is->vidclk, &is->audclk, &is->extclk);*/
	}
#endif
}
