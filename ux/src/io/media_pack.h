#ifndef UX_IO_PACK_MEDIA_H_
#define UX_IO_PACK_MEDIA_H_

#include <boost/smart_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define MAX_DATA_SIZE 900


#define AUDIO 1
#define VIDEO 2

#pragma pack(1)

typedef struct TRcvShakHand {//多媒体接收握手协议
	uint32_t Cmd;    //命令号：默认15
	uint32_t UserID;  //用户ID
} TRcvShakHand;

typedef struct TRcvLeave { //多媒体接收退出接收协议
	uint32_t Cmd;    //命令号：默认17
	uint32_t UserID;
} TRcvLeave;

typedef struct TMediaPack {  //多媒体包
	uint8_t Cmd;    //命令号：默认20
	uint32_t KeyID;   //通讯Key，默认0
	uint8_t SessionID;  //默认0
	uint32_t ID;   //包ID ，递增+1
	uint8_t IsReq;  //是否为请求补发的包 默认为false
	uint32_t UserID;  //用户ID
	uint32_t Time; //发送时间戳
	uint8_t Flag;    //默认0
	uint16_t Size;   //实际数据大小
} TMediaPack;


typedef struct  TVideoFramePackHeader { //视频帧结构
	uint8_t Cmd;  //命令号，固定2
	uint32_t Index;  //ID 需要依次+1
	uint8_t Mode; //视频模式 (vm160 = 0, vm176=1, vm240=2, vm256=3, vm320=4, vm352=5, vm480=6, vm640=7, vmNone=8)
	uint32_t Tick;  //时间戳
	uint8_t IsKeyFrame;  //是否为关键帧数据
	uint32_t FrameSize;  //包大小
	uint16_t sID; //小包id
	uint16_t sCount; //小包数量
	uint32_t sSize; //包大小
} TVideoFramePackHeader;

typedef struct  TAudioFramePackHeader{  //音频帧包头
	uint8_t Cmd;  //命令号，固定为1
	uint8_t AudioMode;  // 音频编码模式 0：AAC+   1:MP3
	uint32_t Index;   //序号 ，需要依次+1
	uint32_t Tick;   //时间戳
	uint16_t Size;   //大小
} TAudioFramePackHeader;


typedef struct TMediaPackReq {//重发请求
	uint8_t Cmd;    //命令号：默认21
	uint32_t ID;   //包ID
	uint32_t UserID;  //用户ID
	uint32_t N1;  //备用
} TMediaPackReq;


#pragma pack() 

class MediaPack : public boost::enable_shared_from_this< MediaPack >
{

private:
	void MallocData(int len)
	{
		if (data_ != NULL)
		{
			free(data_);
		}
		data_ = (uint8_t*)malloc(len);
		memset(data_, 0, len);
	}
public:
	typedef boost::shared_ptr<MediaPack> MediaPackPtr;

	static MediaPackPtr EncodeRcvShakHandPack(uint32_t user_id)
	{
		TRcvShakHand pack = { 0 };
		pack.Cmd = 15;
		pack.UserID = user_id;
		return boost::make_shared<MediaPack>(pack);
	}

	static MediaPackPtr EncodeRcvLeave(uint32_t user_id)
	{
		TRcvLeave pack = { 0 };
		pack.Cmd = 15;
		pack.UserID = user_id;
		return boost::make_shared<MediaPack>(pack);
	}

	static MediaPackPtr EncodeMediaPackReq(uint32_t user_id,int pack_id)
	{
		TMediaPackReq pack = { 0 };
		pack.Cmd = 21;
		pack.UserID = user_id;
		pack.ID = pack_id;
		return boost::make_shared<MediaPack>(pack);
	}

	static MediaPackPtr DecodeBuffer(uint8_t *data, int len)
	{
		MediaPackPtr pack_ptr = boost::make_shared<MediaPack>();
		pack_ptr->Decode(data, len);
		return pack_ptr;
	}

	MediaPack()
	{
	}
	
	MediaPack(TRcvShakHand &p)
	{
		//分配空间
		total_len_ = sizeof(TRcvShakHand);
		MallocData(total_len_);

		memcpy(data_, (void *)&p, total_len_);
	}
	MediaPack(TRcvLeave &p)
	{
		//分配空间
		total_len_ = sizeof(TRcvLeave);
		MallocData(total_len_);

		memcpy(data_, (void *)&p, total_len_);
	};

	MediaPack(TMediaPackReq &p)
	{
		//分配空间
		total_len_ = sizeof(TMediaPackReq);
		MallocData(total_len_);

		memcpy(data_, (void *)&p, total_len_);
	};
	

	void Decode(uint8_t *buffer, int len)
	{
		assert(buffer[0] == 20);
		MallocData(len);
		total_len_ = len;
		memcpy(data_, buffer, len);
		media_pack_ = (TMediaPack*)data_;
		av_type = *((uint8_t*)data_ + sizeof(TMediaPack));
		if (av_type == AUDIO)
		{
			//音频数据包
			media_pack_audio_ = (TAudioFramePackHeader *)(data_ + sizeof(TMediaPack));
		}
		else if (av_type == VIDEO)
		{
			//视频数据包
			media_pack_video_ = (TVideoFramePackHeader *)(data_ + sizeof(TMediaPack));
		}
	}

	~MediaPack()
	{
		if (data_ != NULL)
		{
			free(data_);
			data_ = NULL;
		}
	};

	const uint8_t * data() const
	{
		return data_;
	}

	uint8_t* data()
	{
		return data_;
	}

	uint8_t * body()
	{
		if (av_type == AUDIO)
		{
			//音频数据包
			return (uint8_t*)(data_ + sizeof(TMediaPack)+sizeof(TAudioFramePackHeader));
		}
		else  if(av_type == VIDEO)
		{
			//视频数据包
			return (uint8_t*)(data_ + sizeof(TMediaPack)+sizeof(TVideoFramePackHeader));

		}
		else
		{
			return NULL;
		}

	}

	size_t length() const
	{
		return total_len_;
	}

	int GetId() const
	{
		if (av_type == AUDIO)
		{
			//音频数据包
			return media_pack_audio_->Index;
		}
		else  if(av_type == VIDEO)
		{
			//视频数据包
			return media_pack_video_->Index;

		}
		else
		{
			return 0;
		}
	}

	bool IsSmallPack()
	{
		if (av_type == AUDIO)
		{
			return false;
		}
		else  if (av_type == VIDEO)
		{
			//视频数据包
			return media_pack_video_->sCount>1;
		}
		else
		{
			return false;
		}
	}

	int GetSmallId() const
	{
		if (av_type == AUDIO)
		{
			//音频数据包
			return media_pack_audio_->Index;
		}
		else  if (av_type == VIDEO)
		{
			//视频数据包
			return media_pack_video_->sID;

		}
		else
		{
			return 0;
		}

	}

	int GetDataSize() const
	{
		if (av_type == AUDIO)
		{
			//音频数据包
			return media_pack_audio_->Size;
		}
		else  if (av_type == VIDEO)
		{
			//视频数据包
			return media_pack_video_->sSize;

		}
		else
		{
			return 0;
		}
	}
	int GetStartId() const
	{
		return 0;

	}

	int GetFrameSize()
	{
		if (av_type == AUDIO)
		{
			//音频数据包
			return 0;
		}
		else  if (av_type == VIDEO)
		{
			//视频数据包
			return media_pack_video_->FrameSize;

		}
		else
		{
			return 0;
		}
	}

	int GetCount() const
	{
		if (av_type == AUDIO)
		{
			//音频数据包
			return 0;
		}
		else  if (av_type == VIDEO)
		{
			//视频数据包
			return media_pack_video_->sCount;

		}
		else
		{
			return 0;
		}
	}

	int GetType()
	{
		return av_type;
	}

private:
	TMediaPack  *media_pack_ = NULL;
	TAudioFramePackHeader  *media_pack_audio_ = NULL;
	TVideoFramePackHeader  *media_pack_video_ = NULL;
	uint8_t av_type = 0;
	uint8_t *data_ = NULL;
	size_t total_len_;
};
#endif UX_IO_PACK_MEDIA_H_