#ifndef UX_JSON_PACK_H_
#define UX_JSON_PACK_H_

//��ʵ�ֵ�ͷ�ļ�

//c��ͷ�ļ�

//c++��ͷ�ļ�
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
//�������ͷ�ļ�
#include <boost/cstdint.hpp>
//����Ŀ��ͷ�ļ�

using boost::uint64_t;
using boost::uint32_t;
using boost::uint16_t;
using boost::uint8_t;

using boost::int64_t;
using boost::int32_t;
using boost::int16_t;
using boost::int8_t;

class JsonPack
{
public:
	JsonPack();
	~JsonPack();
	const char* data() const
	{
		return data_;
	}

	char* data()
	{
		return data_;
	}

	size_t length() const
	{
		if (type == pack_type_pack)
		{
			return header_length() + body_length_;
		}
		else
		{
			return header_length() + body_length_;
		}
	}

	const char* body() const
	{
		return data_ + header_length();
	}

	char* body()
	{
		return data_ + header_length();
	}

	size_t header_length() const
	{
		if (type == pack_type_pack)
		{
			return header_pack_length;
		}
		else
		{
			return header_anspack_length;
		}
	}

	size_t body_length() const
	{
		return body_length_;
	}

	void body_length(size_t new_length)
	{
		body_length_ = new_length;
		if (body_length_ > max_body_length)
			body_length_ = max_body_length;
	}

	bool decode_cmd_header()
	{
		//using namespace std; // For strncat and atoi.
		//char header[header_length + 1] = "";
		//strncat(header, data_, header_length);
		//body_length_ = atoi(header);
		//if (body_length_ > max_body_length)
		//{
		//	body_length_ = 0;
		//	return false;
		//}
		return true;
	}

	bool decode_pack_header()
	{
		//using namespace std; // For strncat and atoi.
		//char header[header_length + 1] = "";
		//strncat(header, data_, header_length);
		//body_length_ = atoi(header);
		//if (body_length_ > max_body_length)
		//{
		//	body_length_ = 0;
		//	return false;
		//}
		return true;
	}

	bool decode_anspack_header()
	{
		//using namespace std; // For strncat and atoi.
		//char header[header_length + 1] = "";
		//strncat(header, data_, header_length);
		//body_length_ = atoi(header);
		//if (body_length_ > max_body_length)
		//{
		//	body_length_ = 0;
		//	return false;
		//}
		return true;
	}

	void encode_cmd_header()
	{
		//using namespace std; // For sprintf and memcpy.
		//char header[header_length + 1] = "";
		//sprintf(header, "%4d", body_length_);
		//memcpy(data_, header, header_length);
	}

	void encode_pack_header()
	{
		//using namespace std; // For sprintf and memcpy.
		//char header[header_length + 1] = "";
		//sprintf(header, "%4d", body_length_);
		//memcpy(data_, header, header_length);
	}

	void encode_anspack_header()
	{
		//using namespace std; // For sprintf and memcpy.
		//char header[header_length + 1] = "";
		//sprintf(header, "%4d", body_length_);
		//memcpy(data_, header, header_length);
	}
private:
	typedef struct TJsonPackHead
	{
		int8_t Falg; //������ 0:BIn 1:Xml 2:XmlW
		int8_t Key;
		int8_t Cmd;
	} TJsonPackHead;

	typedef struct TJsonPack
	{
		TJsonPackHead HD;
		int16_t Channel;//: Word; //ͨ��ID
		int32_t ID;		//: LongWord; //��ID ������+1
		int8_t NeedAns;	//: Boolean; //�Ƿ���Ҫ�ظ�
		int32_t Time;	//: Cardinal; //ʱ���
		int32_t bID;	//: LongWord; //���ID
		int32_t sID;	//: LongWord; //��ʼ��ID
		int32_t eID;	//: LongWord; //����ID
		uint16_t Size;	//: Word;
		int8_t *Data;	//: array[0..0] of byte;
	} TJsonPack;

	typedef struct TAnsJsonPack
	{
		TJsonPackHead HD;
		int32_t ID;//: DWORD;
		int32_t Action;//: Integer;
		int32_t crc;//: Cardinal;
	} TAnsJsonPack;
public:
	enum { header_cmd_length = sizeof(TJsonPackHead), header_pack_length = sizeof(TJsonPack), header_anspack_length = sizeof(TAnsJsonPack) };
	enum { max_body_length = 900+1 };
	enum JsonPackType{ pack_type_pack = 0, pack_type_anspack = 1};
private:
	char data_[header_pack_length + max_body_length];
	JsonPackType type = pack_type_pack;
	size_t body_length_;
};
#endif UX_JSON_PACK_H_

