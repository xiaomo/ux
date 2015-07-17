#include "io/connection_udp.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/interprocess/detail/atomic.hpp>

#include "io/pack.h"

ConnectionUDP::ConnectionUDP(boost::shared_ptr< Hive > hive)
: m_hive(hive), m_socket(hive->GetService()), m_io_strand(hive->GetService()), m_timer(hive->GetService()),m_timer_interval(100),
work_thread(boost::bind(&ConnectionUDP::Run,boost::ref(*this)))
{
}

ConnectionUDP::~ConnectionUDP()
{
}

void ConnectionUDP::StartSend(PackPtr pack, bool re_send)
{
	if (!re_send)
	{
		send_pending_list.push_back(pack);
	}
	m_socket.async_send_to(
		boost::asio::buffer(pack->data(), pack->length()),
		remote_ep,
		m_io_strand.wrap(boost::bind(&ConnectionUDP::HandleSend, shared_from_this(), boost::asio::placeholders::error, pack)));

}

void ConnectionUDP::StartRecv()
{
	m_socket.async_receive_from(
		boost::asio::buffer(recv_buffer, sizeof(recv_buffer)/sizeof(uint8_t)),
		remote_ep,
		m_io_strand.wrap(boost::bind(&ConnectionUDP::HandleRecv, shared_from_this(), _1, _2)));
}

void ConnectionUDP::StartTimer()
{
	m_last_time = boost::posix_time::microsec_clock::local_time();
	m_timer.expires_from_now(boost::posix_time::milliseconds(m_timer_interval));
	m_timer.async_wait(m_io_strand.wrap(boost::bind(&ConnectionUDP::HandleTimer, shared_from_this(), _1)));
}


void ConnectionUDP::HandleOpen(const boost::system::error_code & error)
{
	//open之后马上开始接收
	StartRecv();
}

void ConnectionUDP::HandleSend(const boost::system::error_code & error, PackPtr pack)
{
	if (!error)
	{
		pack->SendSuccessOnce();
	}
}

void ConnectionUDP::RemovePackFromSendList(PackPtr pack)
{
	auto result=std::find(send_pending_list.begin(), send_pending_list.end(), pack);
	if (result != send_pending_list.end())
	{
		send_pending_list.erase(result);
	}
}

void ConnectionUDP::HandleRecv(const boost::system::error_code & error, int32_t actual_bytes)
{

	if (m_hive->HasStopped())
	{
		return;
	}
	if (!error && actual_bytes>0)
	{
		//收到数据包啦，进行解析,判断类型
		PackPtr pack = Pack::DecodeBuffer(recv_buffer, actual_bytes);
		
		//回复包,表示对方已经收到了，就更新队列
		if (pack->IsAnsPack())
		{
			RemovePackFromSendList(pack);
		}
		else if (!pack->IsComplete())
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
	//for (auto it = send_pending_list.begin(); it != send_pending_list.end(); )
	//{
	//	PackPtr &pack = *it;
	//	//遍历发送队列，重发或者删除
	//	if (pack->out_of_resend_count())
	//	{
	//		//最大重发次数内
	//		if (pack->should_resend())
	//		{
	//			//到了需要重发的时间
	//			StartSend(*it, false);
	//		}
	//		++it;
	//	}
	//	else
	//	{
	//		//达到最大重发次数,删除
	//		send_pending_list.erase(it);
	//	}
	//}
	//StartTimer();
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
	m_io_strand.post(boost::bind(&ConnectionUDP::HandleClose, shared_from_this()));
	work_thread.join();
}

void ConnectionUDP::PostRecv()
{
	m_io_strand.post(boost::bind(&ConnectionUDP::StartRecv, shared_from_this()));
}

void ConnectionUDP::PostSend(void *json_data, int len)
{
	std::vector<PackPtr> list;
	Pack::EncodeData(json_data, len, list);
	for (auto it = list.begin(); it!=list.end(); ++it)
	{
		m_io_strand.post(boost::bind(&ConnectionUDP::StartSend, shared_from_this(), *it, false));
	}
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