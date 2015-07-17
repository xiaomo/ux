#ifndef VIDEO_DECODER_H_
#define VIDEO_DECODER_H_
#include <stdio.h>
#include "av_codec.h"
#include "video_frame.h"
#include "video_packet.h"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
class VideoDecoder
{
	public:
		wokan::CodecID _id;
		virtual void Feed(boost::shared_ptr<wokan::VideoPacket> video_packet)=0;

		template<typename CallBack>
		void RegisterVideoDecoderCallback(CallBack cb)
		{
			this->_cb = cb;
		}

		virtual ~VideoDecoder()
		{
		}
	protected:
		typedef boost::function<void(boost::shared_ptr<wokan::VideoFrame> video_frame)> callback_t;
		callback_t _cb = NULL;
};
#endif /* VIDEO_DECODER_H_ */
