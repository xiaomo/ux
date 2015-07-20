#ifndef UX_IO_PACK_MEDIA_H_
#define UX_IO_PACK_MEDIA_H_

#include <boost/smart_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define MAX_DATA_SIZE 900


#define AUDIO 1
#define VIDEO 2

#pragma pack(1)

typedef struct TRcvShakHand {//��ý���������Э��
	uint32_t Cmd;    //����ţ�Ĭ��15
	uint32_t UserID;  //�û�ID
} TRcvShakHand;

typedef struct TRcvLeave { //��ý������˳�����Э��
	uint32_t Cmd;    //����ţ�Ĭ��17
	uint32_t UserID;
} TRcvLeave;

typedef struct TMediaPack {  //��ý���
	uint8_t Cmd;    //����ţ�Ĭ��20
	uint32_t KeyID;   //ͨѶKey��Ĭ��0
	uint8_t SessionID;  //Ĭ��0
	uint32_t ID;   //��ID ������+1
	uint8_t IsReq;  //�Ƿ�Ϊ���󲹷��İ� Ĭ��Ϊfalse
	uint32_t UserID;  //�û�ID
	uint32_t Time; //����ʱ���
	uint8_t Flag;    //Ĭ��0
	uint16_t Size;   //ʵ�����ݴ�С
} TMediaPack;


typedef struct  TVideoFramePackHeader { //��Ƶ֡�ṹ
	uint8_t Cmd;  //����ţ��̶�2
	uint32_t Index;  //ID ��Ҫ����+1
	uint8_t Mode; //��Ƶģʽ (vm160 = 0, vm176=1, vm240=2, vm256=3, vm320=4, vm352=5, vm480=6, vm640=7, vmNone=8)
	uint32_t Tick;  //ʱ���
	uint8_t IsKeyFrame;  //�Ƿ�Ϊ�ؼ�֡����
	uint32_t FrameSize;  //����С
	uint16_t sID; //С��id
	uint16_t sCount; //С������
	uint32_t sSize; //����С
} TVideoFramePackHeader;

typedef struct  TAudioFramePackHeader{  //��Ƶ֡��ͷ
	uint8_t Cmd;  //����ţ��̶�Ϊ1
	uint8_t AudioMode;  // ��Ƶ����ģʽ 0��AAC+   1:MP3
	uint32_t Index;   //��� ����Ҫ����+1
	uint32_t Tick;   //ʱ���
	uint16_t Size;   //��С
} TAudioFramePackHeader;


typedef struct TMediaPackReq {//�ط�����
	uint8_t Cmd;    //����ţ�Ĭ��21
	uint32_t ID;   //��ID
	uint32_t UserID;  //�û�ID
	uint32_t N1;  //����
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
		//����ռ�
		total_len_ = sizeof(TRcvShakHand);
		MallocData(total_len_);

		memcpy(data_, (void *)&p, total_len_);
	}
	MediaPack(TRcvLeave &p)
	{
		//����ռ�
		total_len_ = sizeof(TRcvLeave);
		MallocData(total_len_);

		memcpy(data_, (void *)&p, total_len_);
	};

	MediaPack(TMediaPackReq &p)
	{
		//����ռ�
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
			//��Ƶ���ݰ�
			media_pack_audio_ = (TAudioFramePackHeader *)(data_ + sizeof(TMediaPack));
		}
		else if (av_type == VIDEO)
		{
			//��Ƶ���ݰ�
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
			//��Ƶ���ݰ�
			return (uint8_t*)(data_ + sizeof(TMediaPack)+sizeof(TAudioFramePackHeader));
		}
		else  if(av_type == VIDEO)
		{
			//��Ƶ���ݰ�
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
			//��Ƶ���ݰ�
			return media_pack_audio_->Index;
		}
		else  if(av_type == VIDEO)
		{
			//��Ƶ���ݰ�
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
			//��Ƶ���ݰ�
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
			//��Ƶ���ݰ�
			return media_pack_audio_->Index;
		}
		else  if (av_type == VIDEO)
		{
			//��Ƶ���ݰ�
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
			//��Ƶ���ݰ�
			return media_pack_audio_->Size;
		}
		else  if (av_type == VIDEO)
		{
			//��Ƶ���ݰ�
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
			//��Ƶ���ݰ�
			return 0;
		}
		else  if (av_type == VIDEO)
		{
			//��Ƶ���ݰ�
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
			//��Ƶ���ݰ�
			return 0;
		}
		else  if (av_type == VIDEO)
		{
			//��Ƶ���ݰ�
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