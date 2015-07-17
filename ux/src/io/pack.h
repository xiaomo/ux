#ifndef UX_IO_PACK_H_
#define UX_IO_PACK_H_

#include <boost/date_time/posix_time/posix_time.hpp>

#define MAX_RESEND_COUNT 300

#define RESEND_DELAY 200

class Pack
{
private:
	typedef struct TPackHead
	{
		int8_t Falg; //包类型 0:BIn 1:Xml 2:XmlW
		int8_t Key;
		int8_t Cmd;
	} TPackHead;

	typedef struct TPack
	{
		TPackHead HD;
		int16_t Channel;//: Word; //通道ID
		int32_t ID;		//: LongWord; //包ID ，递增+1
		int8_t NeedAns;	//: Boolean; //是否需要回复
		int32_t Time;	//: Cardinal; //时间戳
		int32_t bID;	//: LongWord; //大包ID
		int32_t sID;	//: LongWord; //开始包ID
		int32_t eID;	//: LongWord; //结束ID
		uint16_t Size;	//: Word;
		int8_t *Data;	//: array[0..0] of byte;
	} TPack;

	typedef struct TAnsPack
	{
		TPackHead HD;
		int32_t ID;//: DWORD;
		int32_t Action;//: Integer;
		int32_t crc;//: Cardinal;
	} TAnsPack;
public:
	enum PackType{ kPack = 0, kAnspack = 1 };
public:

	static Pack & decode(uint8_t *data, int len)
	{
		Pack pack;
		return pack;
	}

	Pack(){};
	virtual ~Pack(){};



	bool decode_header() {
		return true;
	};

	void encode_header(PackType pack_type, int body_len)
	{
		type = pack_type;
		if (type == kPack)
		{

		}
		else if (type == kAnspack)
		{

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

	size_t length() const
	{
		return header_length_ + body_length_;
	}

	const uint8_t* body() const
	{
		return data_ + header_length_;
	}

	uint8_t* body()
	{
		return data_ + header_length_;
	}

	size_t body_length() const
	{
		return body_length_;
	}

	void body_length(size_t new_length)
	{

	}

	bool IsAnsPack()
	{
		return type == kAnspack;
	}

	bool IsComplete()
	{
		return true;
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
	PackType type = kPack;
	uint8_t *data_=NULL;
	size_t header_length_;
	size_t body_length_;
	size_t sended_count_;
	boost::posix_time::ptime last_send_time_;
};
#endif UX_IO_PACK_H_