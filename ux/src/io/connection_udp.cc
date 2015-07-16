#include "io/connection_udp.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/interprocess/detail/atomic.hpp>

ConnectionUDP::ConnectionUDP(boost::shared_ptr< Hive > hive)
: m_hive(hive), m_socket(hive->GetService()), m_io_strand(hive->GetService()), m_timer(hive->GetService()),m_timer_interval(1000),
work_thread(boost::bind(ConnectionUDP::Run,boost::ref(*this)))
{
}

ConnectionUDP::~ConnectionUDP()
{
}

void ConnectionUDP::StartSend(Pack &pack, bool re_send)
{
	if (!re_send)
	{
		m_pending_sends.push_back(pack);
	}
	m_socket.async_send_to(
		boost::asio::buffer(pack.data(), pack.length()),
		remote_ep,
		m_io_strand.wrap(boost::bind(&ConnectionUDP::HandleSend, shared_from_this(), boost::asio::placeholders::error, pack)));

}

void ConnectionUDP::StartRecv()
{
	m_socket.async_receive_from(
		boost::asio::buffer(recv_buffer.data(), recv_buffer.length()),
		remote_ep,
		m_io_strand.wrap(boost::bind(&ConnectionUDP::HandleRecv, shared_from_this(), _1, _2)));
}

void ConnectionUDP::StartTimer()
{
	m_last_time = boost::posix_time::microsec_clock::local_time();
	m_timer.expires_from_now(boost::posix_time::milliseconds(m_timer_interval));
	m_timer.async_wait(m_io_strand.wrap(boost::bind(&ConnectionUDP::HandleTimer, shared_from_this(), _1)));
}


void ConnectionUDP::HandleOpen()
{
	//open之后马上开始接收
	StartRecv();
}

void ConnectionUDP::HandleSend(const boost::system::error_code & error, Pack &pack)
{
	if (!error)
	{
		pack.SendSuccessOnce();
	}
}

void ConnectionUDP::HandleRecv(const boost::system::error_code & error, Pack &pack)
{
	if (m_hive->HasStopped())
	{
		return;
	}
	if (!error && pack.length>0)
	{
		//收到数据包啦


		//进行解析,判断类型
		pack.decode_header();

		//回复包就更新队列
		if (pack.IsAnsPack())
		{

		}
		else if (!pack.IsComplete())
		{
			//从接收不完整包队列里面查找，如果找到就更新，并判断是否完成，完整了从队列里面移除

			//如果找不到就放到队列
			recved_uncomplete_list.push_back(pack);
		}
		else
		{
			//完整包就送出去
			if (data_callback_)
			{
				data_callback_(pack);
			}
		}
	}
	//继续收
	StartRecv();
}

void ConnectionUDP::HandleTimer(const boost::system::error_code & error)
{
	
	StartTimer();
}

void ConnectionUDP::HandleClose()
{
	boost::system::error_code ec;
	m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	m_socket.close(ec);
	m_timer.cancel(ec);
}

void ConnectionUDP::Run()
{
	while (!exited)
	{
		m_hive->Poll();
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}
}

void ConnectionUDP::Open(const std::string & host, uint16_t port)
{
	boost::system::error_code ec;
	boost::asio::ip::udp::resolver resolver(m_hive->GetService());
	boost::asio::ip::udp::resolver::query query(host, boost::lexical_cast< std::string >(port));
	boost::asio::ip::udp::resolver::iterator iterator = resolver.resolve(query);
	remote_ep = *iterator;
	m_socket.open(remote_ep.protocol(), ec);
	m_io_strand.post(boost::bind(&ConnectionUDP::HandleOpen, shared_from_this(),ec));
	StartTimer();
}

void ConnectionUDP::Close()
{
	m_io_strand.post(boost::bind(&ConnectionUDP::HandleClose, shared_from_this(), boost::asio::error::connection_reset));
	work_thread.join();
}

void ConnectionUDP::PostRecv()
{
	m_io_strand.post(boost::bind(&ConnectionUDP::StartRecv, shared_from_this()));
}

void ConnectionUDP::PostSend(Pack &pack)
{

	m_io_strand.post(boost::bind(&ConnectionUDP::StartSend, shared_from_this(), pack));
}

boost::asio::ip::udp::socket & ConnectionUDP::GetSocket()
{
	return m_socket;
}

boost::asio::strand & ConnectionUDP::GetStrand()
{
	return m_io_strand;
}

boost::shared_ptr< Hive > ConnectionUDP::GetHive()
{
	return m_hive;
}

int32_t ConnectionUDP::GetTimerInterval() const
{
	return m_timer_interval;
}

void ConnectionUDP::SetTimerInterval(int32_t timer_interval)
{
	m_timer_interval = timer_interval;
}