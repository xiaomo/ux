#ifndef UX_UX_H_
#define UX_UX_H_

#include <string>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>

#include "io/hive.h"
#include "io/connection_udp.h"
#include "io/connection_udp_media.h"

#include "media/audio_track.h"
#include "media/video_render.h"

typedef boost::shared_ptr<VideoRender> VideoRenderPtr;
typedef boost::shared_ptr<AudioTrack> AudioTrackPtr;


//û��ʵ����Ŀ���е������ռ�
class UX
{
public:
	UX();
	~UX();
	void StartHall(std::string &ip, int port);
	void StartRoom(std::string &ip, int port);
	void AddMedia(std::string &ip, int port, int user_id, VideoRenderPtr video_render, AudioTrackPtr audio_track);

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
	void OnDataCallback(char *data);

protected:
	typedef boost::shared_ptr<Hive> HivePtr;
	typedef boost::shared_ptr<ConnectionUDP> ConnectPtr;
	typedef boost::shared_ptr<ConnectionUDPMedia> ConnectMediaPtr;
	typedef boost::function<void(char *data)> HallDataCallback;
	typedef boost::function<void(int roomid,char *data)> RoomDataCallback;
private:
	HivePtr hive_ptr;
	ConnectPtr hall_connect,room_connect;
	std::vector<ConnectMediaPtr> media_connect_vector;
	HallDataCallback hall_data_callback_ = NULL;
	RoomDataCallback room_data_callback_ = NULL;

};

#endif // UX_UX_H_