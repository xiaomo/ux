#ifndef UX_NETWORK_CONNECTION_UDP_H_
#define UX_NETWORK_CONNECTION_UDP_H_

#include "io/pack.h"


#include <string>
#include <vector>
#include <list>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <boost/bimap.hpp>
#include <boost/thread.hpp>

#include "io/hive.h"

using boost::uint64_t;
using boost::uint32_t;
using boost::uint16_t;
using boost::uint8_t;

using boost::int64_t;
using boost::int32_t;
using boost::int16_t;
using boost::int8_t;


class ConnectionUDP;

//-----------------------------------------------------------------------------

class ConnectionUDP : public boost::enable_shared_from_this< ConnectionUDP >
{
	friend class Hive;
protected:
	typedef boost::function<void(Pack &pack)> DataCallback;
private:
	boost::shared_ptr< Hive > m_hive;
	boost::asio::ip::udp::socket m_socket;
	boost::asio::strand m_io_strand;

	boost::asio::ip::udp::endpoint remote_ep;
	boost::asio::deadline_timer m_timer;
	boost::posix_time::ptime m_last_time;
	
	std::list<Pack> m_pending_sends;
	std::list<Pack> recved_uncomplete_list;

	Pack recv_buffer;

	int32_t m_timer_interval;

	bool exited;
	boost::thread work_thread;

	DataCallback data_callback_;
public:
	ConnectionUDP(boost::shared_ptr< Hive > hive);
	virtual ~ConnectionUDP();

private:
	ConnectionUDP(const ConnectionUDP & rhs);
	ConnectionUDP & operator =(const ConnectionUDP & rhs);

	//�����ķ���
	void StartSend(Pack &pack,bool re_send);
	void StartRecv();
	void StartTimer();
	
	//�շ���ɻص�
	void HandleOpen();
	void HandleSend(const boost::system::error_code & error, Pack &pack);
	void HandleRecv(const boost::system::error_code & error, Pack &pack);
	void HandleTimer(const boost::system::error_code & error);
	void HandleClose();

	//�����߳�
	void Run();

public:

	template<typename T>
	inline void RegisterDataCallback(T cb)
	{
		this->data_callback_ = cb;
	}

	boost::shared_ptr< Hive > GetHive();

	boost::asio::ip::udp::socket & GetSocket();

	boost::asio::strand & GetStrand();

	void SetTimerInterval(int32_t timer_interval_ms);

	int32_t GetTimerInterval() const;

	void Open(const std::string & host, uint16_t port);

	void PostSend(Pack &pack);
	
	void PostRecv();

	void Close();
};

#endif //UX_NETWORK_CONNECTION_UDP_H_