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
	//open֮�����Ͽ�ʼ����
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
		//�յ����ݰ��������н���,�ж�����
		PackPtr pack = Pack::DecodeBuffer(recv_buffer, actual_bytes);
		
		//�ظ���,��ʾ�Է��Ѿ��յ��ˣ��͸��¶���
		if (pack->IsAnsPack())
		{
			RemovePackFromSendList(pack);
		}
		else if (!pack->IsComplete())
		{
			//�ӽ��ղ�����������������ң�����ҵ��͸��£����ж��Ƿ���ɣ������˴Ӷ��������Ƴ�

			//����Ҳ����ͷŵ�����
			recved_uncomplete_list.push_back(pack);
		}
		else
		{
			//���������ͳ�ȥ
			if (data_callback_)
			{
				data_callback_(pack);
			}
		}
	}
	//������
	StartRecv();
}

void ConnectionUDP::HandleTimer(const boost::system::error_code & error)
{
	//for (auto it = send_pending_list.begin(); it != send_pending_list.end(); )
	//{
	//	PackPtr &pack = *it;
	//	//�������Ͷ��У��ط�����ɾ��
	//	if (pack->out_of_resend_count())
	//	{
	//		//����ط�������
	//		if (pack->should_resend())
	//		{
	//			//������Ҫ�ط���ʱ��
	//			StartSend(*it, false);
	//		}
	//		++it;
	//	}
	//	else
	//	{
	//		//�ﵽ����ط�����,ɾ��
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