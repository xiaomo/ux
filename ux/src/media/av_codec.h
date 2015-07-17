#ifndef AV_CODEC_H_
#define AV_CODEC_H_

namespace wokan
{
	enum CodecID
	{
		CODEC_NONE = -1, CODEC_AUDIO_AAC, CODEC_AUDIO_SPEEX, CODEC_AUDIO_OPUS, CODEC_VIDEO_H264
	};

	enum FormatPixel
	{
		PIXEL_FMT_NONE = -1, PIXEL_FMT_NV21
	};
}
#endif /* AV_CODEC_H_ */
