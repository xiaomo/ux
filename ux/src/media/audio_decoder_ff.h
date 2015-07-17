#ifndef AUDIO_DECODER_FF_H_
#define AUDIO_DECODER_FF_H_
extern "C"
{
	#include <libavutil/opt.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/channel_layout.h>
	#include <libavutil/common.h>
	#include <libavutil/imgutils.h>
	#include <libavutil/mathematics.h>
	#include <libavutil/samplefmt.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavutil/pixfmt.h>
	#include <libswresample/swresample.h>
}
#include "audio_decoder.h"
#include "concurrent_queue.hpp"
#include "av_codec.h"
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

class AudioDecoderFF: public AudioDecoder
{
	public:
		// Stereo, 32 kHz, 60 ms (2 * 32 * 60)
		static const int kMaxDataSizeSamples = 3840*2;
		AudioDecoderFF(wokan::CodecID id, int sample_rate, int nb_channels);
		void Feed(boost::shared_ptr<wokan::AudioPacket>  audio_packet);
		virtual ~AudioDecoderFF();
	private:
		bool CheckSampleFmt(AVCodec *codec, enum AVSampleFormat sample_fmt);
		void Decode();
		void thread_exit_callback();
		ConcurrentQueue<boost::shared_ptr<wokan::AudioPacket>> queue;
		uint8_t *decode_buffer;
		AVCodec *codec;
		AVCodecContext *codec_ctx;
		struct SwrContext *swr_ctx;
		AVPacket avpkt;
		AVFrame *decoded_frame;
		boost::thread work_thread;


		char * filename = "C:\\Users\Mo\Desktop\rec.pcm";
		FILE *file = NULL;
		bool save2file = false;
};

#endif /* AUDIO_DECODER_FF_H_ */
