#include "ux.h"

UX::UX():
hive_ptr(new Hive()),
hall_connect(new ConnectionUDP(hive_ptr)),
room_connect(new ConnectionUDP(hive_ptr))
{
	hall_connect->RegisterDataCallback(boost::bind(&UX::OnDataCallback,boost::ref(*this),_1));
	video_decoder = boost::make_shared<VideoDecoderFF>(wokan::CodecID::CODEC_VIDEO_H264);
	audio_decoder = boost::make_shared<AudioDecoderFF>(wokan::CodecID::CODEC_AUDIO_AAC, 8000, 1);
	video_decoder->RegisterVideoDecoderCallback(boost::bind(&UX::OnVideoDecoded, boost::ref(*this), _1));
	audio_decoder->RegisterAudioDecoderCallback(boost::bind(&UX::OnAudioDecoded, boost::ref(*this), _1));
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

void UX::AddMedia(std::string &ip, int port,int user_id)
{
	ConnectMediaPtr one_channel = boost::make_shared<ConnectionUDPMedia>(hive_ptr);
	media_connect_vector.push_back(one_channel);
	one_channel->Open(media_connect_vector.size()-1,ip, port, user_id);
	one_channel->RegisterDataCallback(boost::bind(&UX::OnMediaDataCallback, boost::ref(*this), _1,_2,_3,_4));
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

//得到数据直接使用
void UX::OnMediaDataCallback(int index, int type, uint8_t *data, int len)
{
	if (media_data_callback_ != NULL)
	{
		media_data_callback_(index,type,data,len);
	}
	if (type == 1)
	{
		//audio
		boost::shared_ptr<wokan::AudioPacket> audio_packet = boost::make_shared<wokan::AudioPacket>();
		audio_packet->data_len = len;
		audio_packet->data = data;
		//audio_packet->timestamp = frame->frame_info.absolute_timestamp;
		audio_packet->id = wokan::CODEC_AUDIO_AAC;
		//memcpy((uint8_t*)audio_packet->data, (uint8_t*)frame->frame_stream + 16, audio_packet->data_len);
		audio_decoder->Feed(audio_packet);
	}
	else if (type==2)
	{
		//video
		boost::shared_ptr<wokan::VideoPacket> packet = boost::make_shared<wokan::VideoPacket>();
		packet->id = wokan::CodecID::CODEC_VIDEO_H264;
		packet->data_len = len;
		packet->data = data;
		packet->frame_type = 0;
		video_decoder->Feed(packet);
	}
}

void UX::OnVideoDecoded(boost::shared_ptr<wokan::VideoFrame> video_frame)
{
	std::cout << "OnVideoDecoded" << std::endl;
}
void UX::OnAudioDecoded(boost::shared_ptr<wokan::AudioFrame> audio_frame)
{
	std::cout << "OnAudioDecoded" << std::endl;
}
void UX::Exit()
{
	hive_ptr->Stop();
	media_connect_vector.clear();
}