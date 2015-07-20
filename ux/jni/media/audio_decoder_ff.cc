#include "media/audio_decoder_ff.h"
#include <boost/shared_ptr.hpp>
#include <boost/function/function_fwd.hpp>
#include <boost/mem_fn.hpp>
#include <boost/ref.hpp>
#include <boost/type.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "util/utility.hpp"

#define LOGE printf
extern "C" void av_log_callback(void* ptr, int level, const char* fmt, va_list vl)
{
	LOGE(fmt,vl);
}

AudioDecoderFF::AudioDecoderFF(wokan::CodecID id, int sample_rate, int nb_channels) :
		queue(), decode_buffer(NULL), avpkt(), codec(NULL), codec_ctx(NULL), swr_ctx(NULL), decoded_frame(NULL)
{
	if (save2file)
	{
		remove(filename);
#ifdef _WIN32
		fopen_s(&file,filename, "wb");
#else
		file=fopen(filename, "wb");
#endif
		if (!file)
		{
			LOGE("Could not open %s\n", filename);
			exit(1);
		}
	}

	_id = id;
	av_log_set_callback(av_log_callback);
	//step 1. register
	avcodec_register_all();
	switch (_id)
	{
		case wokan::CODEC_AUDIO_AAC:
			codec = avcodec_find_decoder(AV_CODEC_ID_AAC);
			//codec = avcodec_find_decoder_by_name("libfdk_aac");
			break;
		case wokan::CODEC_AUDIO_OPUS:
			codec = avcodec_find_decoder(AV_CODEC_ID_OPUS);
			//codec = avcodec_find_decoder_by_name("libopus");
			break;
		case wokan::CODEC_AUDIO_SPEEX:
			codec = avcodec_find_decoder_by_name("libspeex");
			break;
		default:
			LOGE("not support audio codec");
			break;
	}
	if (!codec)
	{
		LOGE("audio codec not found");
		exit(1);
	}
	//step 2. alloc condec_ctx
	codec_ctx = avcodec_alloc_context3(codec);
	if (!codec_ctx)
	{
		LOGE("could not allocate audio codec context");
		exit(1);
	}
	codec_ctx->channels=nb_channels;
	codec_ctx->sample_rate = sample_rate;

	//	codec_ctx->sample_fmt = AV_SAMPLE_FMT_S16;
	//
	//	if (CheckSampleFmt(codec, codec_ctx->sample_fmt))
	//	{
	//		LOGE("decoder does not support sample format %s", av_get_sample_fmt_name(codec_ctx->sample_fmt));
	//		exit(1);
	//	}

	//step 3. open codec
	int ret=avcodec_open2(codec_ctx, codec, NULL);
	if ( ret< 0)
	{
		LOGE("audio decoder not open codec,err:%d",ret);
		exit(1);
	}
	//step 4. init something
	av_init_packet(&avpkt);
	decoded_frame = av_frame_alloc();
	if (!decoded_frame)
	{
		LOGE("audio decoder not allocate audio frame");
		exit(1);
	}
	decode_buffer = (uint8_t*) malloc(kMaxDataSizeSamples);
	work_thread = make_thread(boost::bind(&AudioDecoderFF::Decode, boost::ref(*this)));
	LOGE("audio decoder init");
}

void AudioDecoderFF::Feed(boost::shared_ptr<wokan::AudioPacket> audio_packet)
{
	queue.push(audio_packet);
}

bool AudioDecoderFF::CheckSampleFmt(AVCodec *codec, enum AVSampleFormat sample_fmt)
{
	const enum AVSampleFormat *p = codec->sample_fmts;
	while (*p != AV_SAMPLE_FMT_NONE)
	{
		//LOGE("suport fmt:%s", av_get_sample_fmt_name(*p));
		p++;
	}
	p = codec->sample_fmts;
	while (*p != AV_SAMPLE_FMT_NONE)
	{
		if (*p == sample_fmt)
			return true;
		p++;
	}
	return false;
}

AudioDecoderFF::~AudioDecoderFF()
{
	work_thread.interrupt();
	work_thread.join();
	boost::shared_ptr<wokan::AudioPacket> audio_packet;
	while (queue.try_pop(audio_packet))
	{

	}
	if (swr_ctx)
	{
		swr_free(&swr_ctx);
	}

	if (codec_ctx)
	{
		avcodec_close(codec_ctx);
		av_free(codec_ctx);
	}
	if (decoded_frame)
	{
		//av_freep(&decoded_frame->data[0]);
		av_frame_free(&decoded_frame);

	}
	free(decode_buffer);
	decode_buffer = NULL;


	if (save2file)
	{
		if (file)
			fclose(file);
	}

	LOGE("audio decoder dispose");
}

void AudioDecoderFF::thread_exit_callback()
{
}
void AudioDecoderFF::Decode()
{
	boost::this_thread::at_thread_exit(boost::bind(&AudioDecoderFF::thread_exit_callback, boost::ref(this)));

	while (true)
	{
		boost::shared_ptr<wokan::AudioPacket> audio_packet;
		queue.wait_and_pop(audio_packet);
		avpkt.data = audio_packet->data;
		avpkt.size = audio_packet->data_len;
		while (avpkt.size > 0)
		{
			int got_frame = 0;
			int len = avcodec_decode_audio4(codec_ctx, decoded_frame, &got_frame, &avpkt);
			boost::this_thread::interruption_point();
			if (len < 0)
			{
				LOGE("decode Error while decoding");
				break;
			}
			if (got_frame)
			{
				//TODO::to reset align
				int data_size = av_samples_get_buffer_size(NULL, codec_ctx->channels, decoded_frame->nb_samples, codec_ctx->sample_fmt, 1);
//				LOGE("decode fmt,%s", av_get_sample_fmt_name(codec_ctx->sample_fmt));
//				LOGE("channels,%d", codec_ctx->channels);
//				LOGE("nb_samples,%d", decoded_frame->nb_samples);
//				LOGE("sample_rate,%d", codec_ctx->sample_rate);
//				LOGE("data_size,%d", data_size);

				if (data_size < 0)
				{
					/* This should not occur, checking just for paranoia */
					LOGE("Failed to calculate data size");
					break;
				}
				if (!swr_ctx)
				{
					swr_ctx = swr_alloc();
					if (!swr_ctx)
					{
						LOGE("Could not allocate swr context");
						exit(1);
					}
					/* set options */
					av_opt_set_int(swr_ctx, "in_channel_layout", codec_ctx->channel_layout, 0);
					av_opt_set_int(swr_ctx, "in_sample_rate", codec_ctx->sample_rate, 0);
					av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", codec_ctx->sample_fmt, 0);

					av_opt_set_int(swr_ctx, "out_channel_layout", codec_ctx->channel_layout, 0);
					av_opt_set_int(swr_ctx, "out_sample_rate", codec_ctx->sample_rate, 0);
					av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

					if (swr_init(swr_ctx) < 0)
					{
						LOGE("Failed to initialize the resampling context");
						exit(1);
					}
				}

				int ret = swr_convert(swr_ctx, &decode_buffer, decoded_frame->nb_samples, (const uint8_t **) &decoded_frame->data[0],
						decoded_frame->nb_samples);
				if (ret < 0)
				{
					LOGE("Error while converting");
					break;
				}
				int data_len = av_samples_get_buffer_size(NULL, codec_ctx->channels, ret, AV_SAMPLE_FMT_S16, 0);
				if (_cb)
				{
					boost::shared_ptr<wokan::AudioFrame> audio_frame = boost::make_shared<wokan::AudioFrame>();
					audio_frame->len_per_sample = 2;
					audio_frame->data_len = data_len;
					audio_frame->data = (uint8_t*) malloc(audio_frame->data_len);
					memcpy(audio_frame->data, decode_buffer, audio_frame->data_len);
					_cb(audio_frame);
					if(save2file)
					{
						//LOGE("write pcm,%d",audio_frame->data_len);
						fwrite(audio_frame->data,1,audio_frame->data_len,file);
						fflush(file);
					}
				}
			}
			avpkt.size -= len;
			avpkt.data += len;
			avpkt.dts = avpkt.pts = AV_NOPTS_VALUE;
			//			if (avpkt.size < AUDIO_REFILL_THRESH)
			//			{
			//				/* Refill the input buffer, to avoid trying to decode
			//				 * incomplete frames. Instead of this, one could also use
			//				 * a parser, or use a proper container format through
			//				 * libavformat. */
			//				memmove(inbuf, avpkt.data, avpkt.size);
			//				avpkt.data = inbuf;
			//				len = fread(avpkt.data + avpkt.size, 1, AUDIO_INBUF_SIZE - avpkt.size, f);
			//				if (len > 0)
			//					avpkt.size += len;
			//			}
		}
	}
}
