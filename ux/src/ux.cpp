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

void UX::AddMedia(std::string &ip, int port)
{
	ConnectPtr one_channel = boost::make_shared<ConnectionUDP>(hive_ptr);
	media_connect_vector.push_back(one_channel);
	one_channel->Open(ip, port);
}

void UX::SendHallData(void *json_data, int len)
{
	hall_connect->PostSend(json_data,len);
}

void UX::SendRoomData(void *json_data, int len)
{
	hall_connect->PostSend(json_data, len);
}

void UX::SendMediaData(int index_media, void *media_data, int len)
{
	media_connect_vector[index_media]->PostSend(media_data, len);
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
	media_connect_vector.clear();
	hive_ptr->Stop();
}