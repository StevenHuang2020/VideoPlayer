// ***********************************************************/
// read_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// read packets thread. 
// ***********************************************************/

#include "read_thread.h"


extern int infinite_buffer;
extern int64_t start_time;
static int64_t duration = AV_NOPTS_VALUE;


ReadThread::ReadThread(QObject* parent, VideoState* pState)
	: QThread(parent)
	, m_pPlayData(pState)
{
}

ReadThread::~ReadThread()
{
}

void ReadThread::set_video_state(VideoState* pState)
{
	assert(pState);
	m_pPlayData = pState;
}

int ReadThread::loop_read()
{
	int ret = -1;
	VideoState* is = m_pPlayData;
	AVPacket* pkt = nullptr;
	int pkt_in_play_range = 0;
	int64_t stream_start_time = 0;
	int64_t pkt_ts = 0;
	//AVFormatContext* pFormatCtx = is->ic;
	assert(is);
	//assert(pFormatCtx);
	if (is == nullptr)
		return -1;

	pkt = av_packet_alloc();
	if (!pkt) {
		av_log(nullptr, AV_LOG_FATAL, "Could not allocate packet.\n");
		ret = AVERROR(ENOMEM);
		return -1;
	}

	is->read_thread_exit = 0;

	for (;;)
	{
		if (is->abort_request)
			break;
		if (is->paused != is->last_paused) {
			is->last_paused = is->paused;
			if (is->paused)
				is->read_pause_return = av_read_pause(is->ic);
			else
				av_read_play(is->ic);
		}

		if (is->seek_req) {
			int64_t seek_target = is->seek_pos;
			int64_t seek_min = is->seek_rel > 0 ? seek_target - is->seek_rel + 2 : INT64_MIN;
			int64_t seek_max = is->seek_rel < 0 ? seek_target - is->seek_rel - 2 : INT64_MAX;
			// FIXME the +-2 is due to rounding being not done in the correct direction in generation
			//      of the seek_pos/seek_rel variables

			ret = avformat_seek_file(is->ic, -1, seek_min, seek_target, seek_max, is->seek_flags);
			if (ret < 0) {
				av_log(nullptr, AV_LOG_ERROR, "%s: error while seeking\n", is->ic->url);
			}
			else {
				if (is->audio_stream >= 0)
					packet_queue_flush(&is->audioq);
				if (is->subtitle_stream >= 0)
					packet_queue_flush(&is->subtitleq);
				if (is->video_stream >= 0)
					packet_queue_flush(&is->videoq);
				if (is->seek_flags & AVSEEK_FLAG_BYTE) {
					set_clock(&is->extclk, NAN, 0);
				}
				else {
					set_clock(&is->extclk, seek_target / (double)AV_TIME_BASE, 0);
				}
			}
			is->seek_req = 0;
			is->queue_attachments_req = 1;
			is->eof = 0;
			if (is->paused)
				step_to_next_frame(is);
		}

		if (is->queue_attachments_req) {
			if (is->video_st && is->video_st->disposition & AV_DISPOSITION_ATTACHED_PIC) {
				if ((ret = av_packet_ref(pkt, &is->video_st->attached_pic)) < 0)
					break;
				packet_queue_put(&is->videoq, pkt);
				packet_queue_put_nullpacket(&is->videoq, pkt, is->video_stream);
			}
			is->queue_attachments_req = 0;
		}

		/* if the queue are full, no need to read more */
		if (infinite_buffer < 1 &&
			(is->audioq.size + is->videoq.size + is->subtitleq.size > MAX_QUEUE_SIZE
				|| (stream_has_enough_packets(is->audio_st, is->audio_stream, &is->audioq) &&
					stream_has_enough_packets(is->video_st, is->video_stream, &is->videoq) &&
					stream_has_enough_packets(is->subtitle_st, is->subtitle_stream, &is->subtitleq)))) {
			/* wait 10 ms */
			m_waitMutex.lock();
			//SDL_CondWaitTimeout(is->continue_read_thread, wait_mutex, 10);
			is->continue_read_thread->wait(&m_waitMutex, 10);
			m_waitMutex.unlock();
			continue;
		}

		ret = av_read_frame(is->ic, pkt);
		if (ret < 0) {
			if ((ret == AVERROR_EOF || avio_feof(is->ic->pb)) && !is->eof) {
				if (is->video_stream >= 0)
					packet_queue_put_nullpacket(&is->videoq, pkt, is->video_stream);
				if (is->audio_stream >= 0)
					packet_queue_put_nullpacket(&is->audioq, pkt, is->audio_stream);
				if (is->subtitle_stream >= 0)
					packet_queue_put_nullpacket(&is->subtitleq, pkt, is->subtitle_stream);

				if (is->loop) { //loop
					stream_seek(is, 0, 0, 0);
				}
				else {
					is->eof = 1;
					break; //add steven for auto exit read thread
				}
			}
			if (is->ic->pb && is->ic->pb->error) {
				break;
			}

			m_waitMutex.lock();
			is->continue_read_thread->wait(&m_waitMutex, 10);
			m_waitMutex.unlock();
			continue;
		}
		else {
			is->eof = 0;
		}

		/* check if packet is in play range specified by user, then queue, otherwise discard */
		stream_start_time = is->ic->streams[pkt->stream_index]->start_time;
		pkt_ts = pkt->pts == AV_NOPTS_VALUE ? pkt->dts : pkt->pts;
		pkt_in_play_range = duration == AV_NOPTS_VALUE ||
			(pkt_ts - (stream_start_time != AV_NOPTS_VALUE ? stream_start_time : 0)) *
			av_q2d(is->ic->streams[pkt->stream_index]->time_base) -
			(double)(start_time != AV_NOPTS_VALUE ? start_time : 0) / 1000000
			<= ((double)duration / 1000000);
		if (pkt->stream_index == is->audio_stream && pkt_in_play_range) {
			packet_queue_put(&is->audioq, pkt);
		}
		else if (pkt->stream_index == is->video_stream && pkt_in_play_range
			&& !(is->video_st->disposition & AV_DISPOSITION_ATTACHED_PIC)) {
			packet_queue_put(&is->videoq, pkt);
		}
		else if (pkt->stream_index == is->subtitle_stream && pkt_in_play_range) {
			packet_queue_put(&is->subtitleq, pkt);
		}
		else {
			av_packet_unref(pkt);
		}

		//print_state_info(is);
	}

	av_packet_free(&pkt);
	return 0;
}

void ReadThread::run()
{
	int ret = loop_read();
	if (ret < 0) {
		qDebug("-------- Read  packets thread exit, with error=%d\n", ret);
	}
	else {
		qDebug("-------- Read  packets thread exit.");
	}
}
