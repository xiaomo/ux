#ifndef UX_UX_H_
#define UX_UX_H_

#include <string>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>

#include "io/hive.h"
#include "io/connection_udp.h"
#include "io/connection_udp_media.h"

#include "media/audio_decoder_ff.h"
#include "media/video_decoder_ff.h"

typedef boost::shared_ptr<VideoDecoderFF> VideoDecoderPtr;
typedef boost::shared_ptr<AudioDecoderFF> AudioDecoderPtr;

//没有实现项目独有的命名空间
class UX
{
public:
	UX();
	~UX();
	void StartHall(std::string &ip, int port);
	void StartRoom(std::string &ip, int port);
	void AddMedia(std::string &ip, int port, int user_id);

	void SendHallData(void *json_data,int len);
	void SendRoomData(void *json_data, int len);
	void Exit();

	template<typename T>
	inline void RegisterHallDataCallback(T cb)
	{
		this->hall_data_callback_ = cb;
	}

	template<typename T>
	inline void RegisterRoomDataCallback(T cb)
	{
		this->room_data_callback_ = cb;
	}
	template<typename T>
	inline void RegisterMediaDataCallback(T cb)
	{
		this->media_data_callback_ = cb;
	}

	void OnDataCallback(char *data);
	void OnMediaDataCallback(int index, int type, uint8_t *data, int len);
	

	void OnVideoDecoded(boost::shared_ptr<wokan::VideoFrame> video_frame);
	void OnAudioDecoded(boost::shared_ptr<wokan::AudioFrame> audio_frame);

protected:
	typedef boost::shared_ptr<Hive> HivePtr;
	typedef boost::shared_ptr<ConnectionUDP> ConnectPtr;
	typedef boost::shared_ptr<ConnectionUDPMedia> ConnectMediaPtr;
	typedef boost::function<void(char *data)> HallDataCallback;
	typedef boost::function<void(int roomid,char *data)> RoomDataCallback;
	typedef boost::function<void(int index, int type,uint8_t *data ,int len)> MediaDataCallback;
private:
	HivePtr hive_ptr;
	ConnectPtr hall_connect,room_connect;
	std::vector<ConnectMediaPtr> media_connect_vector;
	HallDataCallback hall_data_callback_ = NULL;
	RoomDataCallback room_data_callback_ = NULL;
	MediaDataCallback media_data_callback_ = NULL;
	VideoDecoderPtr video_decoder;
	AudioDecoderPtr audio_decoder;
};

#endif // UX_UX_H_