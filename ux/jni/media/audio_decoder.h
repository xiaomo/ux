#ifndef UX_MEDIA_AUDIO_DECODER_H_
#define UX_MEDIA_AUDIO_DECODER_H_

#include <stdio.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "media/av_codec.h"
#include "media/audio_frame.h"
#include "media/audio_packet.h"

class AudioDecoder
{
	public:
		wokan::CodecID _id;
		virtual void Feed(boost::shared_ptr<wokan::AudioPacket>  audio_packet)=0;
		template<typename CallBack>
		void RegisterAudioDecoderCallback(CallBack cb)
		{
			this->_cb=cb;
		}
		virtual ~AudioDecoder(){}
	protected:
		typedef boost::function<void(boost::shared_ptr<wokan::AudioFrame>)> callback_t;
		callback_t _cb=NULL;
};

#endif /* AUDIO_DECODER_H_ */
