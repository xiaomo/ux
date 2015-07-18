#ifndef UX_MEDIA_VIDEO_DECODER_H_
#define UX_MEDIA_VIDEO_DECODER_H_
#include <stdio.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "media/av_codec.h"
#include "media/video_frame.h"
#include "media/video_packet.h"

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
#endif /* UX_MEDIA_VIDEO_DECODER_H_ */
