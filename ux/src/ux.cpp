#include "ux.h"

UX::UX():
hive_ptr(new Hive()),
hall_connect(new ConnectionUDP(hive_ptr)),
room_connect(new ConnectionUDP(hive_ptr))
{

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
	//room_connect->SendTo();
}

void UX::SendRoomData(void *json_data, int len)
{
	//room_connect->SendTo();
}

void UX::SendMediaData(int index_media, void *media_data, int len)
{
	//media_connect_vector[index_media]->SendTo()
}

void UX::Exit()
{
	media_connect_vector.clear();
	hive_ptr->Stop();
}