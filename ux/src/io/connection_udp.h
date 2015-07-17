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

typedef boost::shared_ptr<Pack> PackPtr;

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
	typedef boost::function<void(char *)> DataCallback;
private:
	boost::shared_ptr< Hive > m_hive;
	boost::asio::ip::udp::socket m_socket;
	boost::asio::strand m_io_strand;

	boost::asio::ip::udp::endpoint remote_ep;
	boost::asio::deadline_timer m_timer;
	boost::posix_time::ptime m_last_time;
	
	std::list<PackPtr> send_pending_list;
	std::list<PackPtr> recved_uncomplete_list;

	uint8_t recv_buffer[1024];

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

	void RemovePackFromSendList(PackPtr pack);
	void RegroupPack();
	bool CheckSmallPackExist(PackPtr pack);

	//真正的发送
	void StartSend(PackPtr pack, bool re_send);
	void StartRecv();
	void StartTimer();
	
	//收发完成回调
	void HandleOpen(const boost::system::error_code & error);
	void HandleSend(const boost::system::error_code & error, PackPtr pack);
	void HandleRecv(const boost::system::error_code & error, int32_t actual_bytes);
	void HandleTimer(const boost::system::error_code & error);
	void HandleClose();

	//工作线程
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

	void PostSend(void *json_data, int len);
	
	void PostRecv();

	void Close();
};

#endif //UX_NETWORK_CONNECTION_UDP_H_
