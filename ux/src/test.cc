#include "ux.h"

#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/bind.hpp>


void HallDataCallback(char *data)
{
	std::cout << "receive hall data:" << data << std::endl;
}

void RoomDataCallback(int roomid, char *data)
{
	std::cout<<"receive room data:"<<roomid<< " " << data << std::endl;
}

void MediaDataCallback(int index, char *data)
{
	std::cout << "receive media data:" << index << " " << data << std::endl;
}

int main()
{
	typedef boost::shared_ptr<UX> UXPtr;

	//创建ux类
	UXPtr ux_ptr(new UX());
	
	//开始大厅数据通讯
	ux_ptr->RegisterHallDataCallback(boost::bind(HallDataCallback,_1));
	ux_ptr->StartHall(std::string("222.186.3.27"), 4100);

	/*
	//开始房间数据通讯
	ux_ptr->RegisterRoomDataCallback(boost::bind(RoomDataCallback,_1,_2));
	ux_ptr->StartRoom(std::string("222.186.3.27"), 4100);

	//开始房间媒体通信
	ux_ptr->RegisterMediaDataCallback(boost::bind(MediaDataCallback,_1,_2));
	ux_ptr->AddMedia(std::string("222.186.3.27"), 4100);
	*/

	char *json = "{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}";
	ux_ptr->SendHallData(json, strlen(json) + 1);
	
	boost::this_thread::sleep(boost::posix_time::minutes(1));

	ux_ptr->Exit();
	return 0;
}