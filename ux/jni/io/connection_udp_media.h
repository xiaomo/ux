#ifndef UX_IO_CONNECTION_UDP_MEDIA_H_
#define UX_IO_CONNECTION_UDP_MEDIA_H_

#include "io/media_pack.h"


#include <string>
#include <vector>
#include <list>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <boost/bimap.hpp>
#include <boost/thread.hpp>

#include "io/hive.h"
#include "media/audio_decoder_ff.h"
#include "media/video_decoder_ff.h"
#include "media/video_render.h"
#include "media/audio_track.h"

typedef boost::shared_ptr<MediaPack> MediaPackPtr;
typedef boost::shared_ptr<VideoDecoderFF> VideoDecoderPtr;
typedef boost::shared_ptr<AudioDecoderFF> AudioDecoderPtr;
typedef boost::shared_ptr<VideoRender> VideoRenderPtr;
typedef boost::shared_ptr<AudioTrack> AudioTrackPtr;

using boost::uint64_t;
using boost::uint32_t;
using boost::uint16_t;
using boost::uint8_t;

using boost::int64_t;
using boost::int32_t;
using boost::int16_t;
using boost::int8_t;


class ConnectionUDPMedia;

//-----------------------------------------------------------------------------

class ConnectionUDPMedia : public boost::enable_shared_from_this< ConnectionUDPMedia >
{
	friend class Hive;
private:
	boost::shared_ptr< Hive > m_hive;
	boost::asio::ip::udp::socket m_socket;
	boost::asio::strand m_io_strand;

	boost::asio::ip::udp::endpoint remote_ep;
	boost::asio::deadline_timer m_timer;
	boost::posix_time::ptime m_last_time;
	
	std::list<MediaPackPtr> recved_uncomplete_list;

	uint8_t recv_buffer[1024];

	int32_t m_timer_interval;

	bool exited;
	boost::thread work_thread;

	int index;
	int user_id;

	VideoDecoderPtr video_decoder_;
	AudioDecoderPtr audio_decoder_;
	VideoRenderPtr video_render_;
	AudioTrackPtr audio_track_;
public:
	ConnectionUDPMedia(boost::shared_ptr< Hive > hive);
	virtual ~ConnectionUDPMedia();

private:
	ConnectionUDPMedia(const ConnectionUDPMedia & rhs);
	ConnectionUDPMedia & operator =(const ConnectionUDPMedia & rhs);

	void RegroupPack(MediaPackPtr pack);

	//真正的发送
	void StartSend(MediaPackPtr pack);
	void StartRecv();
	void StartTimer();
	
	//收发完成回调
	void HandleOpen(const boost::system::error_code & error);
	void HandleSend(const boost::system::error_code & error, MediaPackPtr pack);
	void HandleRecv(const boost::system::error_code & error, int32_t actual_bytes);
	void HandleTimer(const boost::system::error_code & error);
	void HandleClose();

	//工作线程
	void Run();

	//获取到了音视频
	void OnGotMediaData(int index, int type,int audio_type, uint8_t *data, int len);
	//解码回调
	void OnVideoDecoded(boost::shared_ptr<wokan::VideoFrame> video_frame);
	void OnAudioDecoded(boost::shared_ptr<wokan::AudioFrame> audio_frame);
public:
	boost::shared_ptr< Hive > GetHive();

	boost::asio::ip::udp::socket & GetSocket();

	boost::asio::strand & GetStrand();

	void SetTimerInterval(int32_t timer_interval_ms);

	int32_t GetTimerInterval() const;

	void Open(int index, const std::string & host, uint16_t port, int user_id, VideoRenderPtr video_render, AudioTrackPtr audio_track);

	void PostSendLeave();

	void Close();
};

#endif //UX_IO_CONNECTION_UDP_MEDIA_H_
