#ifndef UX_IO_PACK_H_
#define UX_IO_PACK_H_

class Pack
{
public:
	Pack();
	virtual ~Pack();

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
		return header_length_ + body_length_;
	}

	//const char* body() const
	//{
	//	return data_ + header_length;
	//}

	//char* body()
	//{
	//	return data_ + header_length;
	//}

	//size_t body_length() const
	//{
	//	return body_length_;
	//}

	//virtual void body_length(size_t new_length) = 0;

	bool IsAnsPack()
	{
		return true;
	}

	bool IsComplete()
	{
		return true;
	}

	bool decode_header() {};

	void encode_header() {};
	

	void SendSuccessOnce()
	{
		sended_count++;
	}

private:
	//char data_[header_length + max_body_length];
	char data_[1024];
	size_t body_length_;
	size_t header_length_;
	size_t sended_count;
};
#endif UX_IO_PACK_H_