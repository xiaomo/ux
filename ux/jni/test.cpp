#include "ux.h"

#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/bind.hpp>

extern "C"
{
#pragma comment (lib, "Ws2_32.lib")  
#pragma comment (lib, "avcodec.lib")
#pragma comment (lib, "avdevice.lib")
#pragma comment (lib, "avfilter.lib")
#pragma comment (lib, "avformat.lib")
#pragma comment (lib, "avutil.lib")
#pragma comment (lib, "swresample.lib")
#pragma comment (lib, "swscale.lib")
};

void HallDataCallback(char *data)
{
	std::cout << "receive hall data:"<<boost::this_thread::get_id()<< std::endl;
}

void RoomDataCallback(int roomid, char *data)
{
	std::cout<<"receive room data:"<<roomid<< " " << data << std::endl;
}

void MediaDataCallback(int index, int type, uint8_t *data, int len)
{
	if (type == 1)
	{
		std::cout << "receive audio data:" << index << " " << len <<"-"<< boost::this_thread::get_id() << std::endl;
	}
	else if (type == 2)
	{
		std::cout << "receive video data:" << index << " " << len << "-" << boost::this_thread::get_id() << std::endl;
	}
}

int main()
{
	typedef boost::shared_ptr<UX> UXPtr;

	//����ux��
	UXPtr ux_ptr(new UX());
	
	//��ʼ��������ͨѶ
	ux_ptr->RegisterHallDataCallback(boost::bind(HallDataCallback,_1));
	//ux_ptr->StartHall(std::string("222.186.3.27"), 4100);
	ux_ptr->StartHall(std::string("222.186.57.54"), 5005);
	
	////��ʼ��������ͨѶ
	//ux_ptr->RegisterRoomDataCallback(boost::bind(RoomDataCallback,_1,_2));
	//ux_ptr->StartRoom(std::string("222.186.3.27"), 4100);

	//��ʼ����ý��ͨ��
	ux_ptr->RegisterMediaDataCallback(boost::bind(MediaDataCallback,_1,_2,_3,_4));
	ux_ptr->AddMedia(std::string("222.186.3.27"), 4111, 1002);
	

	char *json = "{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}{\"Mac\":\"1043B56CD666AFCEDFCFC0718C0DB807\",\"Cmd\":10103,\"UserPass\":\"098F6BCD4621D373CADE4E832627B4F6\",\"UserID\":10000}";
	ux_ptr->SendHallData(json, strlen(json));
	
	boost::this_thread::sleep(boost::posix_time::seconds(20));

	ux_ptr->Exit();
	return 0;
}