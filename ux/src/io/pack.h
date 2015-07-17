#ifndef UX_IO_PACK_H_
#define UX_IO_PACK_H_

#include <boost/smart_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define TPack_CMD 1
#define TAnsPack_CMD 2

#define MAX_DATA_SIZE 900

#define MAX_RESEND_COUNT 300
#define RESEND_DELAY 200

#pragma pack(1)
typedef struct TPackHead
{
	int8_t Falg; //包类型 0:BIn 1:Xml 2:XmlW
	int8_t Key;
	int8_t Cmd;
} TPackHead;

typedef struct TPack
{
	int16_t Channel;//: Word; //通道ID
	int32_t ID;		//: LongWord; //包ID ，递增+1
	int8_t NeedAns;	//: Boolean; //是否需要回复
	int32_t Time;	//: Cardinal; //时间戳
	int32_t bID;	//: LongWord; //大包ID
	int32_t sID;	//: LongWord; //开始包ID
	int32_t eID;	//: LongWord; //结束ID
	uint16_t Size;	//: Word;
	uint8_t *Data;	//: array[0..0] of byte;
} TPack;

typedef struct TAnsPack
{
	int32_t ID;//: DWORD;
	int32_t Action;//: Integer;
	int32_t crc;//: Cardinal;
} TAnsPack;

#pragma pack() 

class Pack : public boost::enable_shared_from_this< Pack >
{

private:
	void MallocData(int len)
	{
		if (data_ != NULL)
		{
			free(data_);
		}
		data_ =(uint8_t*)malloc(len);
		memset(data_,0,len);
	}
public:
	typedef boost::shared_ptr<Pack> PackPtr;

	static void EncodeData(void *json_data, int len, std::vector<PackPtr>&list)
	{
		int bid = (current_id++);
		int count = (len + MAX_DATA_SIZE - 1) / MAX_DATA_SIZE;
		std::cout << "开始封包++++++++++" << count << std::endl;
		for (int i = 0; i < count; i++)
		{
			TPack pack = {0};
			pack.ID = bid+1+i;
			pack.NeedAns = 1;
			pack.Time = 0;
			pack.bID = bid;
			pack.sID = bid+1;
			pack.eID = bid+count;
			pack.Size = len>MAX_DATA_SIZE ? MAX_DATA_SIZE : len;
			pack.Data = (uint8_t*)json_data + i * MAX_DATA_SIZE;
			len -= pack.Size;
			list.push_back(boost::make_shared<Pack>(pack));
			std::cout << "封包:" << bid << "/" << pack.ID << " size:" << pack.Size << std::endl;
		}
		std::cout << "结束封包++++++++++" << std::endl;
	}

	static PackPtr EncodeAnsPack(uint32_t id)
	{
		TAnsPack pack = { 0 };
		pack.ID = id;
		pack.Action = 0;
		pack.crc = 0;
		return boost::make_shared<Pack>(pack);
	}

	static PackPtr DecodeBuffer(uint8_t *data, int len)
	{
		PackPtr pack_ptr=boost::make_shared<Pack>();
		pack_ptr->Decode(data, len);
		return pack_ptr;
	}

	Pack()
	{
	}

	Pack(TPack &p)
	{
		//分配空间,最后的+1在最后加个空字符
		total_len_ = sizeof(TPackHead)+sizeof(TPack)-sizeof(uint8_t *)+p.Size+1;
		MallocData(total_len_);
		 
		//拷贝命令头
		pack_header_ = (TPackHead*)(data_);
		pack_header_->Falg = 2;
		pack_header_->Key = 0;
		pack_header_->Cmd = 1;
		//memcpy(data_, (void *)&pack_header_, sizeof(TPackHead));

		//拷贝包信息
		send_pack_header_ = (TPack *)(data_ + sizeof(TPackHead));
		memcpy(send_pack_header_, (void *)&p, sizeof(TPack)-sizeof(uint8_t *));
		
		//拷贝数据
		memcpy(data_ + sizeof(TPackHead)+sizeof(TPack)-sizeof(uint8_t *), (void *)p.Data, p.Size);
	};

	Pack(TAnsPack &p)
	{
		//分配空间
		total_len_ = sizeof(TPackHead)+sizeof(TAnsPack);
		MallocData(total_len_);

		//拷贝命令头
		pack_header_ = (TPackHead*)(data_);
		pack_header_->Falg = 2;
		pack_header_->Key = 0;
		pack_header_->Cmd = 2;
		//memcpy(data_, (void *)&pack_header_, sizeof(TPackHead));

		//拷贝包信息
		ans_pack_header_ = (TAnsPack *)(data_ + sizeof(TPackHead));
		memcpy(data_ + sizeof(TPackHead), (void *)&p, sizeof(TAnsPack));
	};

	void Decode(uint8_t *buffer, int len)
	{
		MallocData(len);
		total_len_ = len;
		memcpy(data_ ,buffer, len);
		pack_header_ = (TPackHead*)data_;
		if (pack_header_->Cmd == TPack_CMD)
		{
			//普通数据包
			send_pack_header_ = (TPack *)(data_ + sizeof(TPackHead));
		}
		else
		{
			//回复包
			ans_pack_header_ = (TAnsPack *)(data_ + sizeof(TPackHead));
		}
	}

	~Pack()
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
		if (IsAnsPack())
		{
			return NULL;
		}
		else
		{
			return data_ + sizeof(TPackHead)+sizeof(TPack)-sizeof(uint8_t *);
		}
		
	}

	size_t length() const
	{
		return total_len_;
	}

	int GetId() const
	{
		if (pack_header_->Cmd == TPack_CMD)
		{
			//普通包
			return send_pack_header_->sID;
		}
		else
		{
			return ans_pack_header_->ID;
			//回复包
		}

	}

	int GetBigId() const
	{
		if (pack_header_->Cmd == TPack_CMD)
		{
			//普通包
			return send_pack_header_->bID;
		}
		else
		{
			return ans_pack_header_->ID;
			//回复包,不应该出现这种情况
		}

	}

	int GetDataSize() const
	{
		if (pack_header_->Cmd == TPack_CMD)
		{
			//普通包
			return send_pack_header_->Size;
		}
		else
		{
			return 0;
		}

	}
	int GetStartId() const
	{
		if (pack_header_->Cmd == TPack_CMD)
		{
			//普通包
			return send_pack_header_->sID;
		}
		else
		{
			return ans_pack_header_->ID;
			//回复包,不应该出现这种情况
		}

	}

	int GetEndId() const
	{
		if (pack_header_->Cmd == TPack_CMD)
		{
			//普通包
			return send_pack_header_->eID;
		}
		else
		{
			return ans_pack_header_->ID;
			//回复包,不应该出现这种情况
		}

	}

	bool IsAnsPack()
	{
		return pack_header_->Cmd == TAnsPack_CMD;
	}
	bool needAns()
	{
		if (IsAnsPack())
		{
			return false;
		}
		else
		{
			send_pack_header_->NeedAns != 0;
		}
	}

	bool IsSmallPack()
	{
		if (IsAnsPack())
		{
			return false;
		}
		else
		{
			send_pack_header_->sID < send_pack_header_->eID;
		}
	}

	void SendSuccessOnce()
	{
		sended_count_++;
		last_send_time_ = boost::posix_time::microsec_clock::local_time();
	}

	boost::posix_time::ptime last_send_time()
	{
		return last_send_time_;
	}

	size_t sended_count()
	{
		return sended_count_;
	}

	bool out_of_resend_count()
	{
		return sended_count_ >= MAX_RESEND_COUNT;
	}

	bool should_resend()
	{
		if (out_of_resend_count())
		{
			//不应该出现这种情况
			assert(!out_of_resend_count());
			return false;
		}
		else if (sended_count_ == 0 || boost::posix_time::microsec_clock::local_time() - last_send_time_ >= boost::posix_time::milliseconds(RESEND_DELAY))
		{
			return true;
		}
		return false;
	}

private:
	TPackHead *pack_header_ = NULL;
	TPack *send_pack_header_ = NULL;
	TAnsPack *ans_pack_header_ = NULL;
	
	uint8_t *data_=NULL;
	size_t header_cmd_len_;
	size_t hender_pack_length_;
	size_t hender_anspack_length_;
	size_t total_len_;

	size_t sended_count_;

	boost::posix_time::ptime last_send_time_;

	static uint32_t current_id;
};



#endif UX_IO_PACK_H_