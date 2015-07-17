#ifndef VIDEO_FRAME_H_
#define VIDEO_FRAME_H_


#include "av_codec.h"
#include <malloc.h>

#include <boost/cstdint.hpp>

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
	class VideoFrame
	{
		public:
			VideoFrame(){

			};
			~VideoFrame()
			{
				if(data)
				{
					free(data);
					data=NULL;
				}
			};
			uint8_t * data = NULL;
			uint32_t data_len=0;
			uint64_t timeStamp=0;
			uint32_t width=0;
			uint32_t height=0;
			bool need_iframe=false;
			wokan::FormatPixel fmt = wokan::FormatPixel::PIXEL_FMT_NV21;
	};
}
#endif /* VIDEO_FRAME_H_ */
