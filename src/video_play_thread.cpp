#include "video_play_thread.h"


VideoPlayThread::VideoPlayThread(QObject* parent)
	: QThread(parent),
	m_bExitThread(false),
	m_bPauseThread(false),
	m_bWaitToExitThread(false)
{
	m_FrameBufferQueue.clear();
	m_frameRate = 25; // 25 frame per second
	m_videoBufferSize = int(1 * m_frameRate); // size of frame nums in buffer
	m_playSleep = int(1000 * 1.0 / m_frameRate); // milliseconds
}

VideoPlayThread::~VideoPlayThread()
{
	stop_thread();
	clear_image_queue();
}

//const QImage* VideoPlayThread::get_image()
//{
//	if (!m_FrameBufferQueue.isEmpty())
//		return m_FrameBufferQueue.dequeue();
//	return NULL;
//}

uint VideoPlayThread::get_buffersize()
{
	return m_FrameBufferQueue.length();
}

void VideoPlayThread::print_buffer_info()
{
	qDebug("video buffer packets: %d", get_buffersize());
}

void VideoPlayThread::clear_image_queue()
{
	QMutexLocker lock(&m_mutex);
	qDebug("frame buffer length:%d", m_FrameBufferQueue.length());
	m_FrameBufferQueue.clear();
	lock.unlock();
}

void VideoPlayThread::receive_image(const QImage& img)
{
	m_mutex.lock();
	if (get_buffersize() >= m_videoBufferSize)  //limit buffer size
	{
		m_bufferNotFull.wait(&m_mutex);
	}
	m_mutex.unlock();

	QMutexLocker lock(&m_mutex);
	m_FrameBufferQueue.enqueue(img);
	m_bufferNotEmpty.wakeAll();
	lock.unlock();

#if PRINT_VIDEO_BUFFER_INFO
	print_buffer_info();
#endif
}

void VideoPlayThread::run()
{
	while (true)
	{
		if (m_bExitThread)
			break;

		if (m_bWaitToExitThread && get_buffersize() == 0)
			break;

		m_mutex.lock();
		if (get_buffersize() == 0)
		{
			m_bufferNotEmpty.wait(&m_mutex);
		}
		m_mutex.unlock();

		if (m_bPauseThread || m_FrameBufferQueue.isEmpty())
		{
			QThread::msleep(10);
		}
		else
		{
#if PRINT_VIDEO_BUFFER_INFO
			print_buffer_info();
#endif
			// const QImage* pImage = get_image();
			const QImage& img = m_FrameBufferQueue.dequeue();
			//show image
			emit show_image(img);

			m_mutex.lock();
			m_bufferNotFull.wakeAll();
			m_mutex.unlock();

			QThread::msleep(m_playSleep); //control play frame rate
		}
	}
	qDebug("--------video play thread exit.");
}

void VideoPlayThread::stop_thread()
{
	m_bExitThread = true;
	m_bufferNotEmpty.wakeAll();
	wait();
}

void VideoPlayThread::wait_stop_thread()
{
	m_bWaitToExitThread = true;
	m_bufferNotEmpty.wakeAll();
	wait();
}

void VideoPlayThread::pause_thread()
{
	m_bPauseThread = !m_bPauseThread;
	m_bufferNotFull.wakeAll();
}
