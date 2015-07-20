#ifndef UX_MEDIA_VIDEO_PACKET_H_
#define UX_MEDIA_VIDEO_PACKET_H_
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
	class VideoPacket
	{
		public:
			VideoPacket(){};
			virtual ~VideoPacket()
			{
				if(data)
				{
					free(data);
					data=NULL;
				}
			}
			uint64_t timestamp=0;
			uint8_t *data=NULL;
			uint16_t data_len=0;
			CodecID id=wokan::CodecID::CODEC_NONE;
			int frame_type;
			bool iframe=false;
	};
}
#endif /* UX_MEDIA_VIDEO_PACKET_H_ */
