#include <assert.h>
#include "ffmpeg_decode.h"


/**
 * Write the given AVFrame into a .ppm file.
 *
 * @param   avFrame     the AVFrame to be saved.
 * @param   width       the given frame width as obtained by the AVCodecContext.
 * @param   height      the given frame height as obtained by the AVCodecContext.
 * @param   frameIndex  the given frame index.
 */
void CallbackFrame(AVFrame* avFrame, int width, int height, int frameIndex, void* pContext)
{
    FILE* pFile;
    char szFilename[32];
    int  y;

    /**
     * We do a bit of standard file opening, etc., and then write the RGB data.
     * We write the file one line at a time. A PPM file is simply a file that
     * has RGB information laid out in a long string. If you know HTML colors,
     * it would be like laying out the color of each pixel end to end like
     * #ff0000#ff0000.... would be a red screen. (It's stored in binary and
     * without the separator, but you get the idea.) The header indicated how
     * wide and tall the image is, and the max size of the RGB values.
     */
        
    printf("width:%d, height:%d, frameIndex:%d, pContext:%p\n", width, height, frameIndex, pContext);
    return;

    // Open file
    sprintf(szFilename, "frame%d.ppm", frameIndex);
    pFile = fopen(szFilename, "wb");
    if (pFile == NULL)
    {
        return;
    }

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for (y = 0; y < height; y++)
    {
        fwrite(avFrame->data[0] + y * avFrame->linesize[0], 1, width * 3, pFile);
    }

    // Close file
    fclose(pFile);
}

static void log_callback(void* ptr, int level, const char* fmt, va_list vl)
{
    if (level > av_log_get_level())
        return;

    va_list vl2;
    char line[1024];
    static int print_prefix = 1;

    va_copy(vl2, vl);
    av_log_format_line(ptr, level, fmt, vl2, line, sizeof(line), &print_prefix);
    va_end(vl2);

    printf("FFMPEG:%s\n", line);
}

Video_decode::Video_decode(decoded_video_callback cb_fun_video,
    decoded_audio_callback cb_fun_audio, void* pContext) 
{
	m_pFormatCtx = NULL;
    m_videoStream = -1;
    m_audioStream = -1;
    m_pVideoCodecCtx = NULL;
    m_pAudioCodecCtx = NULL;

    callback_video_fun = cb_fun_video;
    callback_audio_fun = cb_fun_audio;
    // callback_fun = CallbackFrame;
    m_pCallContext = pContext;

    m_bExit = false;
    m_bPause = false;

    av_log_set_level(32);
    av_log_set_callback(log_callback);
}

Video_decode::~Video_decode() {
    release_decode();
}


void Video_decode::release_decode() {
    // Close the codecs
    if (m_pVideoCodecCtx) {
        avcodec_close(m_pVideoCodecCtx);
        m_pVideoCodecCtx = NULL;
    }
    
    if (m_pAudioCodecCtx) {
        avcodec_close(m_pAudioCodecCtx);
        m_pAudioCodecCtx = NULL;
    }

    // Close the video file
    if (m_pFormatCtx) {
        avformat_close_input(&m_pFormatCtx);
        m_pFormatCtx = NULL;
    }
}

const AVCodecContext* Video_decode::get_decode_context(bool bVideo)
{
    if (bVideo)
    {
        return m_pVideoCodecCtx;
    }
    else {
        return m_pAudioCodecCtx;
    }
}

bool Video_decode::init_decode(const char* filename)
{
    int ret = avformat_open_input(&m_pFormatCtx, filename, NULL, NULL);    // [2]
    if (ret < 0)
    {
        printf("Could not open file %s\n", filename);
        return false;
    }

    // Retrieve stream information
    ret = avformat_find_stream_info(m_pFormatCtx, NULL);  //[3]
    if (ret < 0)
    {
        printf("Could not find stream information %s\n", filename);
        return false;
    }

    // information about file onto standard error
    av_dump_format(m_pFormatCtx, 0, filename, 0);  // [4]

    // Now pFormatCtx->streams is just an array of pointers, of size
    // pFormatCtx->nb_streams, so let's walk through it until we find a video
    // stream.
    unsigned int i;

    // The stream's information about the codec is in what we call the
    // "codec context." This contains all the information about the codec that
    // the stream is using
   
    // Find the first video stream
    for (i = 0; i < m_pFormatCtx->nb_streams; i++)
    {
        // check the General type of the encoded data to match
        if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_videoStream = i;
            m_pVideoCodecCtx = open_codec_contex(i);
 
        }
        else if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            m_audioStream = i;
            m_pAudioCodecCtx = open_codec_contex(i);
        }

        if (m_videoStream != -1 && m_audioStream != -1)
            break;
    }

    if (m_pVideoCodecCtx == NULL && m_pAudioCodecCtx == NULL)
        return false;

    return true;
}

AVCodecContext* Video_decode::open_codec_contex(int streamId)
{
    if (streamId < 0) {
        return NULL;
    }
    // Find the decoder for the video stream
    AVCodecID codec_id = m_pFormatCtx->streams[streamId]->codecpar->codec_id;
    const AVCodec* pCodec = avcodec_find_decoder(codec_id);
    if (pCodec == NULL)
    {
        // codec not found
        printf("Unsupported codec!\n");
        return NULL;
    }

    AVCodecContext* pContex = avcodec_alloc_context3(pCodec);
    int ret = avcodec_parameters_to_context(pContex, m_pFormatCtx->streams[streamId]->codecpar);
    if (ret != 0)
    {
        // error copying codec context
        printf("Could not copy codec context.\n");
        return NULL;
    }

    // Open codec
    ret = avcodec_open2(pContex, pCodec, NULL);
    if (ret < 0)
    {
        // Could not open codec
        printf("Could not open codec.\n");
        return NULL;
    }

    return pContex;
}

int Video_decode::start_decode() {
    assert(m_pFormatCtx != NULL); // if is NULL, please init_decode first
    //assert(m_videoStream >= 0);
    //assert(m_pVideoCodecCtx != NULL);
    //assert(m_pAudioCodecCtx != NULL);

    // Now we need a place to actually store the frame:
    AVFrame* pFrame = NULL;

    // Allocate video frame
    pFrame = av_frame_alloc();
    if (pFrame == NULL)
    {
        printf("Could not allocate frame.\n");
        return -1;
    }

     // Allocate a rgb video frame
    AVFrame* pFrameRGB = NULL;
    uint8_t* buffer_RGB = NULL;

    if (m_pVideoCodecCtx) {
        pFrameRGB = av_frame_alloc();
        if (pFrameRGB == NULL)
        {
            printf("Could not allocate rgb frame.\n");
            return -1;
        }

        int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
            m_pVideoCodecCtx->width, m_pVideoCodecCtx->height, 32);
        buffer_RGB = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

        av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer_RGB,
            AV_PIX_FMT_RGB24, m_pVideoCodecCtx->width, m_pVideoCodecCtx->height, 32);
    }
   
   //malloc buffer for decoed audio
    struct SwrContext* swrCtx = NULL;
    if (m_pAudioCodecCtx) {
        swrCtx = swr_alloc_set_opts(NULL,
            m_pAudioCodecCtx->channel_layout, AV_SAMPLE_FMT_S16, m_pAudioCodecCtx->sample_rate,
            m_pAudioCodecCtx->channel_layout, m_pAudioCodecCtx->sample_fmt, m_pAudioCodecCtx->sample_rate,
            0, nullptr);
        swr_init(swrCtx);
    }
    

    AVPacket* pPacket = av_packet_alloc();
    if (pPacket == NULL)
    {
        printf("Could not alloc packet,\n");
        return -1;
    }

    /**
     * The process, again, is simple: av_read_frame() reads in a packet and
     * stores it in the AVPacket struct. Note that we've only allocated the
     * packet structure - ffmpeg allocates the internal data for us, which
     * is pointed to by packet.data. This is freed by the av_free_packet()
     * later. avcodec_decode_video() converts the packet to a frame for us.
     * However, we might not have all the information we need for a frame
     * after decoding a packet, so avcodec_decode_video() sets
     * frameFinished for us when we have decoded enough packets the next
     * frame.
     * Finally, we use sws_scale() to convert from the native format
     * (pCodecCtx->pix_fmt) to RGB. Remember that you can cast an AVFrame
     * pointer to an AVPicture pointer. Finally, we pass the frame and
     * height and width information to our SaveFrame function.
     */

    int i = 0;
    while (!m_bExit)
    {
        if (m_bPause)
        {
            av_usleep(10000);
            continue;
        }

        if (av_read_frame(m_pFormatCtx, pPacket) < 0)
            break;
               
        // Is this a packet from the video stream?
        if (pPacket->stream_index == m_videoStream)
        {
            if (m_pVideoCodecCtx)
            {
                video_decode(i++, m_pVideoCodecCtx, pFrame, pFrameRGB, pPacket);
            }            
        }
        else if (pPacket->stream_index == m_audioStream) 
        {
            if (m_pAudioCodecCtx)
            {
                audio_decode(m_pAudioCodecCtx, pPacket, pFrame, swrCtx);
            }
        }
   
        av_packet_unref(pPacket);
    }

    printf("Decode loop ended normally.\n");
    /* Cleanup.  */
    //Free decoded audio
    swr_free(&swrCtx);

    // Free the RGB image
    av_free(buffer_RGB);
    av_frame_free(&pFrameRGB);
    av_free(pFrameRGB);

    // Free the YUV frame
    av_frame_free(&pFrame);
    av_free(pFrame);
    return 0;
}

int Video_decode::video_decode(int frameId, AVCodecContext* pContex, AVFrame* pFrame, AVFrame* pFrameRGB, AVPacket* pPacket)
{
    struct SwsContext* sws_ctx = sws_getContext(   // [13]
        pContex->width,
        pContex->height,
        pContex->pix_fmt,
        pContex->width,
        pContex->height,
        AV_PIX_FMT_RGB24,   // sws_scale destination color scheme
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );

    int ret = avcodec_send_packet(pContex, pPacket);
    if (ret < 0)
    {
        printf("Error sending packet for decoding.\n");
        return -1;
    }

    while (ret >= 0)
    {
        ret = avcodec_receive_frame(pContex, pFrame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            break;
        }
        else if (ret < 0)
        {
            printf("Error while decoding.\n");
            return -1;
        }

        // Convert the image from its native format to RGB
        sws_scale(sws_ctx, (uint8_t const* const*)pFrame->data, pFrame->linesize, 0, pContex->height, pFrameRGB->data, pFrameRGB->linesize);

        // save the read AVFrame into ppm file
        if (callback_video_fun)
        {
            callback_video_fun(pContex, pFrameRGB, frameId, m_pCallContext);
        }
        //saveFrame(pFrameRGB, m_pCodecCtx->width, m_pCodecCtx->height, i);

        // print log information
        printf(
            "Frame %c (%d) pts %I64d dts %I64d key_frame %d "
            "[coded_picture_number %d, display_picture_number %d,"
            " %dx%d]\n",
            av_get_picture_type_char(pFrame->pict_type),
            pContex->frame_number,
            pFrameRGB->pts,
            pFrameRGB->pkt_dts,
            pFrameRGB->key_frame,
            pFrameRGB->coded_picture_number,
            pFrameRGB->display_picture_number,
            pContex->width,
            pContex->height
        );
    }
    return 0;
}


int Video_decode::audio_decode(AVCodecContext* dec_ctx, AVPacket* pkt, AVFrame* frame, struct SwrContext* swrCtx)
{
    int ret;

    /* send the packet with the compressed data to the decoder */
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error submitting the packet to the decoder\n");
        return -1;
    }

    /* read all the output frames (in general there may be any number of them */
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return 0;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            return -1;
        }
        //int data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
        //if (data_size < 0) {
        //    /* This should not occur, checking just for paranoia */
        //    return -1;
        //}

        int bufsize = av_samples_get_buffer_size(nullptr, frame->channels, frame->nb_samples, AV_SAMPLE_FMT_S16, 0);
        uint8_t* buffer_audio = (uint8_t*)av_malloc(bufsize * sizeof(uint8_t));
        swr_convert(swrCtx, &buffer_audio, frame->nb_samples, (const uint8_t**)(frame->data), frame->nb_samples);
       

        if (callback_audio_fun) {
            //callback_audio_fun(dec_ctx, frame, data_size, m_pCallContext);
            callback_audio_fun(dec_ctx, frame, bufsize, buffer_audio, m_pCallContext);
        }

        /*for (int i = 0; i < frame->nb_samples; i++)
            for (int ch = 0; ch < dec_ctx->channels; ch++)
                fwrite(frame->data[ch] + data_size * i, 1, data_size, outfile);*/
    }

    return 0;
}

//void Video_decode::decode_init(const char* filename)
//{
//    if (!init_decode(filename))
//    {
//        printf("decode initial failed, file:%s!\n", filename);
//        return;
//    }
//
//    start_decode();
//}

void Video_decode::stop_decode()
{
    m_bExit = true;
}

void Video_decode::pause_decode()
{
    m_bPause = !m_bPause;
}

bool Video_decode::paused()
{
    return m_bPause;
}
