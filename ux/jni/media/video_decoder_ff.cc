#include "media/video_decoder_ff.h"
#include "util/utility.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/function/function_fwd.hpp>
#include <boost/mem_fn.hpp>
#include <boost/ref.hpp>
#include <boost/type.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>

#define LOGE printf

VideoDecoderFF::VideoDecoderFF(wokan::CodecID id)
{
	_id = id;
	avcodec_register_all();
	switch (_id)
	{
		case wokan::CodecID::CODEC_VIDEO_H264:
			codec = avcodec_find_decoder(AV_CODEC_ID_H264);
			break;
		default:
			LOGE("not supported codec type");
			break;
	}
	if (!codec)
	{
		LOGE("video decoder not found\n");
		exit(1);
	}
	codec_ctx = avcodec_alloc_context3(codec);
	if (!codec_ctx)
	{
		LOGE("Could not allocate video codec context\n");
		exit(1);
	}
	//c->time_base.den = 10;
	//c->width = 240;
	//c->height = 320;
	codec_ctx->pix_fmt = PIX_FMT_YUV420P;
	av_opt_set(codec_ctx->priv_data, "preset", "ultrafast", 0);
	av_opt_set(codec_ctx->priv_data, "tune", "zerolatency", 0);
	if (avcodec_open2(codec_ctx, codec, NULL) < 0)
	{
		LOGE("Could not open codec\n");
		exit(1);
	}
	av_init_packet(&avpkt);

	memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

	frame = av_frame_alloc();
	if (!frame)
	{
		LOGE("Could not allocate video frame\n");
		exit(1);
	}

	/*hm_result g_hm_result = hm_video_init(HME_VE_H264, &g_hdl_video_codec);

	if (g_hm_result == HMEC_OK)
	{
		LOGE("Initialize video codec ok");
	}*/

	work_thread = make_thread(boost::bind(&VideoDecoderFF::Decode, boost::ref(*this)));
	LOGE("video decoder init ok");

}

void VideoDecoderFF::Feed(boost::shared_ptr<wokan::VideoPacket> video_packet)
{
	queue.push(video_packet);
}

VideoDecoderFF::~VideoDecoderFF()
{

	work_thread.interrupt();
	work_thread.join();

	boost::shared_ptr<wokan::VideoPacket> video_packet;
	while (queue.try_pop(video_packet))
	{

	}
	if (codec_ctx)
	{
		avcodec_close(codec_ctx);
		av_free(codec_ctx);
		codec = NULL;
		codec_ctx = NULL;

	}
	if (frame)
	{
		//av_freep(&frame->data[0]);
		av_frame_free(&frame);
		frame = NULL;
	};

}
void VideoDecoderFF::Decode()
{
	while (true)
	{
		boost::shared_ptr<wokan::VideoPacket> video_packet;
		queue.wait_and_pop(video_packet);
		avpkt.data = video_packet->data;
		avpkt.size = video_packet->data_len;
		while (avpkt.size > 0 && _cb)
		{

			/*uint64_t time0 = GetCurrentTimeLong();
			hm_result res = hm_video_decode_yuv(g_hdl_video_codec, (pointer) (video_packet->data), video_packet->data_len, &g_yuv_handle);
			if (res == HMEC_OK)
			{
			}
			LOGA("vdecode time:%llu", GetCurrentTimeLong()-time0);

			break;*/
			//codec_ctx->width =240;
			//codec_ctx->height = 320;
			codec_ctx->pix_fmt = PIX_FMT_YUV420P;

			frame->format = codec_ctx->pix_fmt;
			frame->width = codec_ctx->width;
			frame->height = codec_ctx->height;

			av_opt_set(codec_ctx->priv_data, "preset", "ultrafast", 0);
			av_opt_set(codec_ctx->priv_data, "tune", "zerolatency", 0);

			int len, got_frame;
			//uint64_t time = GetCurrentTimeLong();
			len = avcodec_decode_video2(codec_ctx, frame, &got_frame, &avpkt);
			//LOGA("vdecode time:%llu,%d", GetCurrentTimeLong()-time, queue.size());
			boost::this_thread::interruption_point();
			if (len < 0)
			{
				LOGE("Error while decoding frame");
				break;
			}
			if (got_frame)
			{
				int picSize = codec_ctx->height * codec_ctx->width;
				int newSize = picSize * 3 / 2;

				boost::shared_ptr<wokan::VideoFrame> video_frame = boost::make_shared<wokan::VideoFrame>();
				video_frame->fmt = wokan::FormatPixel::PIXEL_FMT_NV21;
				video_frame->data_len = newSize;
				video_frame->height = codec_ctx->height;
				video_frame->width = codec_ctx->width;
				video_frame->data = (uint8_t*) malloc(video_frame->data_len);

				//copy stream
				int a = 0, i;
				for (i = 0; i < codec_ctx->height; i++)
				{
					memcpy(video_frame->data + a, frame->data[0] + i * frame->linesize[0], codec_ctx->width);
					a += codec_ctx->width;
				}

				for (i = 0; i < codec_ctx->height / 2; i++)
				{
					memcpy(video_frame->data + a, frame->data[1] + i * frame->linesize[1], codec_ctx->width / 2);
					a += codec_ctx->width / 2;
				}

				for (i = 0; i < codec_ctx->height / 2; i++)
				{
					memcpy(video_frame->data + a, frame->data[2] + i * frame->linesize[2], codec_ctx->width / 2);
					a += codec_ctx->width / 2;
				}
				_cb(video_frame);
			}
			else{
				LOGE("not got_frame++++++++++ ");
			}
			if (avpkt.data)
			{
				avpkt.size -= len;
				avpkt.data += len;
			}
		}

	}
}
