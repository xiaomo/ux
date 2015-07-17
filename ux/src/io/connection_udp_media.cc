#include "io/connection_udp_media.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/interprocess/detail/atomic.hpp>

#include "io/media_pack.h"

ConnectionUDPMedia::ConnectionUDPMedia(boost::shared_ptr< Hive > hive)
: m_hive(hive), m_socket(hive->GetService()), m_io_strand(hive->GetService()), m_timer(hive->GetService()),m_timer_interval(15000),
work_thread(boost::bind(&ConnectionUDPMedia::Run,boost::ref(*this)))
{
}

ConnectionUDPMedia::~ConnectionUDPMedia()
{
}

void ConnectionUDPMedia::StartSend(MediaPackPtr pack)
{
	m_socket.async_send_to(
		boost::asio::buffer(pack->data(), pack->length()),
		remote_ep,
		m_io_strand.wrap(boost::bind(&ConnectionUDPMedia::HandleSend, shared_from_this(), boost::asio::placeholders::error, pack)));

}

void ConnectionUDPMedia::StartRecv()
{
	m_socket.async_receive_from(
		boost::asio::buffer(recv_buffer, sizeof(recv_buffer)/sizeof(uint8_t)),
		remote_ep,
		m_io_strand.wrap(boost::bind(&ConnectionUDPMedia::HandleRecv, shared_from_this(), _1, _2)));
}

void ConnectionUDPMedia::StartTimer()
{
	m_last_time = boost::posix_time::microsec_clock::local_time();
	m_timer.expires_from_now(boost::posix_time::milliseconds(m_timer_interval));
	m_timer.async_wait(m_io_strand.wrap(boost::bind(&ConnectionUDPMedia::HandleTimer, shared_from_this(), _1)));
}


void ConnectionUDPMedia::HandleOpen(const boost::system::error_code & error)
{
	//open之后马上开始接收
	StartRecv();

	MediaPackPtr pack = MediaPack::EncodeRcvShakHandPack(user_id);
	StartSend(pack);
}

void ConnectionUDPMedia::HandleSend(const boost::system::error_code & error, MediaPackPtr pack)
{
}

void ConnectionUDPMedia::RegroupPack(MediaPackPtr pack)
{
	//插入到队列
	auto it = recved_uncomplete_list.begin();
	if (it == recved_uncomplete_list.end())
	{
		//空队列直接插入
		recved_uncomplete_list.insert(it, pack);
		return;
	}

	int total_count_in_big_pack = pack->GetCount();
	//计算当前大包一共有多少个小包
	int alredy_count = 0;
	int total_size = 0;
	bool inserted = false;

	std::list<MediaPackPtr>::iterator  big_pack_start_it = recved_uncomplete_list.end();

	while (it != recved_uncomplete_list.end())
	{
		MediaPackPtr pack_inlist = *it;
		if (pack_inlist->GetId() == pack->GetId())
		{
			if (big_pack_start_it == recved_uncomplete_list.end())
			{
				//记录第一个小包的位置
				big_pack_start_it = it;
			}
			
			if (!inserted)
			{
				if (pack_inlist->GetSmallId() == pack->GetSmallId())
				{
					//大小包id都相同，丢掉就是
					return;
				}
				else if (pack_inlist->GetSmallId() > pack->GetSmallId())
				{
					recved_uncomplete_list.insert(it, pack);
					inserted = true;
					alredy_count++;
					total_size += pack->GetDataSize();
					big_pack_start_it--;
				}
				else
				{
					++it;
					alredy_count++;
					total_size += pack_inlist->GetDataSize();
				}
			}
			else
			{
				alredy_count++;
				total_size += pack_inlist->GetDataSize();
			}
		}
		else if (pack_inlist->GetId() > pack->GetId() && !inserted)
		{
			recved_uncomplete_list.insert(it, pack);
			alredy_count++;
			total_size += pack->GetDataSize();
		}
		else
		{
			++it;
		}
	}
	//插入到末尾
	if (!inserted)
	{
		recved_uncomplete_list.insert(recved_uncomplete_list.end(), pack);
		alredy_count++;
		total_size += pack->GetDataSize();
	}

	assert(alredy_count <= total_count_in_big_pack);
	assert(total_size <= pack->GetFrameSize());

	if (alredy_count == total_count_in_big_pack)
	{
		char * data = (char*)malloc(total_size);
		//当前大包已经接收完成，需要组装
		int alredy_copy_count = 0;
		for (int i = 0; i < alredy_count; i++)
		{
			MediaPackPtr pack_inlist = *big_pack_start_it;
			memcpy(data + alredy_copy_count, pack_inlist->body(), pack_inlist->GetDataSize());
			alredy_copy_count += pack_inlist->GetDataSize();
			recved_uncomplete_list.erase(big_pack_start_it++);
		}
		std::cout << alredy_count << "个小包组成一个大包：" << total_size << std::endl;
		//完整包就送出去
		if (data_callback_)
		{
			data_callback_(index, pack->GetType(), (uint8_t*)data,total_size);
		}
	}
}

void ConnectionUDPMedia::HandleRecv(const boost::system::error_code & error, int32_t actual_bytes)
{
	if (m_hive->HasStopped())
	{
		return;
	}
	if (!error && actual_bytes>2 && (recv_buffer[0] == 20))
	{
		//收到数据包啦，进行解析,判断类型
		MediaPackPtr pack = MediaPack::DecodeBuffer(recv_buffer, actual_bytes);
		
		if (pack->IsSmallPack())
		{
			//重组包
			RegroupPack(pack);
		}
		else
		{
			//完整包就送出去
			if (data_callback_)
			{
				data_callback_(index,pack->GetType(),(uint8_t *)pack->body(),pack->GetDataSize());
			}
		}
	}
	//继续收
	StartRecv();
}

void ConnectionUDPMedia::HandleTimer(const boost::system::error_code & error)
{
	MediaPackPtr pack = MediaPack::EncodeRcvShakHandPack(user_id);
	StartSend(pack);

	StartTimer();
}

void ConnectionUDPMedia::HandleClose()
{
	boost::system::error_code ec;
	m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	m_socket.close(ec);
	m_timer.cancel(ec);
}

void ConnectionUDPMedia::Run()
{
	while (!exited)
	{
		m_hive->Poll();
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}
}

void ConnectionUDPMedia::Open(int index,const std::string & host, uint16_t port, int user_id)
{
	this->index = index;
	this->user_id = user_id;
	boost::system::error_code ec;
	boost::asio::ip::udp::resolver resolver(m_hive->GetService());
	boost::asio::ip::udp::resolver::query query(host, boost::lexical_cast< std::string >(port));
	boost::asio::ip::udp::resolver::iterator iterator = resolver.resolve(query);
	remote_ep = *iterator;
	m_socket.open(remote_ep.protocol(), ec);
	m_io_strand.post(boost::bind(&ConnectionUDPMedia::HandleOpen, shared_from_this(),ec));
	StartTimer();
}

void ConnectionUDPMedia::Close()
{
	m_io_strand.post(boost::bind(&ConnectionUDPMedia::HandleClose, shared_from_this()));
	work_thread.join();
}

void ConnectionUDPMedia::PostSendLeave()
{
	for (size_t i = 0; i < 10; i++)
	{
		MediaPackPtr pack = MediaPack::EncodeRcvLeave(user_id);
		m_io_strand.post(boost::bind(&ConnectionUDPMedia::StartSend, shared_from_this(), pack));
	}
}

boost::asio::ip::udp::socket & ConnectionUDPMedia::GetSocket()
{
	return m_socket;
}

boost::asio::strand & ConnectionUDPMedia::GetStrand()
{
	return m_io_strand;
}

boost::shared_ptr< Hive > ConnectionUDPMedia::GetHive()
{
	return m_hive;
}

int32_t ConnectionUDPMedia::GetTimerInterval() const
{
	return m_timer_interval;
}

void ConnectionUDPMedia::SetTimerInterval(int32_t timer_interval)
{
	m_timer_interval = timer_interval;
}