#include "ux.h"

UX::UX():
hive_ptr(new Hive()),
hall_connect(new ConnectionUDP(hive_ptr)),
room_connect(new ConnectionUDP(hive_ptr))
{
	hall_connect->RegisterDataCallback(boost::bind(&UX::OnDataCallback,boost::ref(*this),_1));
}

UX::~UX()
{
}

void UX::StartHall(std::string &ip, int port)
{

	hall_connect->Open(ip, port);
}

void UX::StartRoom(std::string &ip, int port)
{
	room_connect->Open(ip, port);
}

void UX::AddMedia(std::string &ip, int port, int user_id, VideoRenderPtr video_render, AudioTrackPtr audio_track)
{
	ConnectMediaPtr one_channel = boost::make_shared<ConnectionUDPMedia>(hive_ptr);
	media_connect_vector.push_back(one_channel);
	one_channel->Open(media_connect_vector.size() - 1, ip, port, user_id, video_render, audio_track);
}

void UX::SendHallData(void *json_data, int len)
{
	hall_connect->PostSend(json_data,len);
}

void UX::SendRoomData(void *json_data, int len)
{
	hall_connect->PostSend(json_data, len);
}

void UX::OnDataCallback(char *data)
{
	if (hall_data_callback_ != NULL)
	{
		hall_data_callback_(data);
	}
}

void UX::Exit()
{
	hive_ptr->Stop();
	media_connect_vector.clear();
}