#ifndef UX_UX_H_
#define UX_UX_H_

#include <string>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>

#include "io/hive.h"
#include "io/connection_udp.h"

//没有实现项目独有的命名空间
class UX
{
public:
	UX();
	~UX();
	void StartHall(std::string &ip, int port);
	void StartRoom(std::string &ip, int port);
	void AddMedia(std::string &ip, int port);

	void SendHallData(void *json_data,int len);
	void SendRoomData(void *json_data, int len);
	void SendMediaData(int index_media, void *media_data, int len);
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

protected:
	typedef boost::shared_ptr<Hive> HivePtr;
	typedef boost::shared_ptr<ConnectionUDP> ConnectPtr;
	typedef boost::function<void(char *data)> HallDataCallback;
	typedef boost::function<void(int roomid,char *data)> RoomDataCallback;
	typedef boost::function<void(int index,char *data)> MediaDataCallback;
private:
	HivePtr hive_ptr;
	ConnectPtr hall_connect,room_connect;
	std::vector<ConnectPtr> media_connect_vector;
	HallDataCallback hall_data_callback_ = NULL;
	RoomDataCallback room_data_callback_ = NULL;
	MediaDataCallback media_data_callback_ = NULL;
};

#endif // UX_UX_H_