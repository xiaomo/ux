#ifndef AUDIO_FRAME_H_
#define AUDIO_FRAME_H_
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
	class AudioFrame
	{
		public:
			AudioFrame()
			{
			}
			virtual ~AudioFrame()
			{
				if (data)
				{
					free(data);
					data=NULL;
				}
			}
			uint64_t timestamp = 0;
			uint8_t *data = NULL;
			int data_len = 0;
			int samples_per_channel;
			int sample_rate_hz;
			int num_channels;
			int len_per_sample;
	};
}
#endif /* AUDIO_FRAME_H_ */
