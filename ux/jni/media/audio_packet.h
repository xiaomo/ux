#ifndef UX_MEDIA_AUDIO_PACKET_H_
#define UX_MEDIA_AUDIO_PACKET_H_

#include <malloc.h>
#include <boost/cstdint.hpp>
#include "media/av_codec.h"

using boost::uint64_t;
using boost::uint32_t;
using boost::uint16_t;
using boost::uint8_t;

using boost::int64_t;
using boost::int32_t;
using boost::int16_t;
using boost::int8_t;

namespace wokan
{
	class AudioPacket
	{
		public:
			AudioPacket():timestamp(0),data(NULL),data_len(0),id(wokan::CodecID::CODEC_NONE){};
			virtual ~AudioPacket()
			{
				if(data)
				{
					free(data);
					data=NULL;
				}
			}
			uint64_t timestamp;
			uint8_t *data;
			uint16_t data_len;
			CodecID id;
	};
}
#endif /* UX_MEDIA_AUDIO_PACKET_H_ */
