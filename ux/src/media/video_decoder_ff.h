#ifndef VIDEO_DECODER_FF_H_
#define VIDEO_DECODER_FF_H_

#include "video_decoder.h"
#include "concurrent_queue.hpp"
#include <boost/thread.hpp>

extern "C"
{
	#include <libavutil/opt.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/channel_layout.h>
	#include <libavutil/common.h>
	#include <libavutil/imgutils.h>
	#include <libavutil/mathematics.h>
	#include <libavutil/samplefmt.h>
}

#define INBUF_SIZE 4096

class VideoDecoderFF: public VideoDecoder
{
	public:
		VideoDecoderFF(wokan::CodecID id);
		void Feed(boost::shared_ptr<wokan::VideoPacket> video_packet);
		~VideoDecoderFF();
	private:
		boost::thread work_thread;
		void Decode();
		ConcurrentQueue<boost::shared_ptr<wokan::VideoPacket>> queue;
		AVCodec *codec=NULL;
		AVCodecContext *codec_ctx = NULL;
		AVFrame *frame=NULL;
		uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
		AVPacket avpkt;
		//video_codec_handle              g_hdl_video_codec = NULL;       // ��Ƶ�������
};
#endif /* VIDEO_DECODER_FF_H_ */
