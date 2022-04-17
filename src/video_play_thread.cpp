#include "video_play_thread.h"


VideoPlayThread::VideoPlayThread(QObject* parent)
	: QThread(parent),
	m_bExitThread(false),
	m_bPauseThread(false)
{
	m_FrameBufferQueue.clear();
	m_frameRate = 25;

	m_playSleep = int(1000 * 1.0 / m_frameRate); // milliseconds
}

VideoPlayThread::~VideoPlayThread()
{
	clear_image_queue();
}

void VideoPlayThread::receive_image(const QImage& img)
{
	QMutexLocker lock(&m_mutex);

	QImage* pImage = new QImage();
	*pImage = img.copy();
	m_FrameBufferQueue.enqueue(pImage);
	lock.unlock();

	print_buffer_info();
}

const QImage* VideoPlayThread::get_image()
{
	QMutexLocker lock(&m_mutex);
	if (!m_FrameBufferQueue.isEmpty())
		return m_FrameBufferQueue.dequeue();
	return NULL;
	lock.unlock();
}

int VideoPlayThread::get_buffersize()
{
	QMutexLocker lock(&m_mutex);
	return m_FrameBufferQueue.length();
	lock.unlock();
}

void VideoPlayThread::print_buffer_info()
{
	qDebug("video buffer packets: %d", get_buffersize());
}

void VideoPlayThread::clear_image_queue()
{
	qDebug("frame buffer length:%d", m_FrameBufferQueue.length());
	while (!m_FrameBufferQueue.isEmpty())
	{
		QImage* pImage = m_FrameBufferQueue.dequeue();
		delete pImage;
	}

	m_FrameBufferQueue.clear();
}

void VideoPlayThread::run()
{
	while (true) 
	{
		if (m_bExitThread && get_buffersize() == 0)
			break;

		const QImage* pImage = get_image();
		if (m_bPauseThread || pImage == NULL)
		{
			msleep(100);
		}
		else {
			//show image
			emit show_image(*pImage);
			delete pImage;

			msleep(m_playSleep * 1.2);
		}
	}
	qDebug("--------video play thread exit.");
	emit finish_play();
}

void VideoPlayThread::stop_thread()
{
	m_bExitThread = true;
	// wait();
}

void VideoPlayThread::pause_thread()
{
	m_bPauseThread = !m_bPauseThread;
}
