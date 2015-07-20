#ifndef UX_IO_HIVE_H_
#define UX_IO_HIVE_H_

#include <boost\smart_ptr.hpp>
#include <boost\asio.hpp>

class Hive : public boost::enable_shared_from_this< Hive >
{
private:
	boost::asio::io_service m_io_service;
	boost::shared_ptr< boost::asio::io_service::work > m_work_ptr;
	volatile uint32_t m_shutdown;

private:
	Hive(const Hive & rhs);
	Hive & operator =(const Hive & rhs);

public:
	Hive();
	virtual ~Hive();

	// Returns the io_service of this object.
	boost::asio::io_service & GetService();

	// Returns true if the Stop function has been called.
	bool HasStopped();

	// Polls the IOing subsystem once from the current thread and 
	// returns.
	void Poll();

	// Runs the IOing system on the current thread. This function blocks 
	// until the IOing system is stopped, so do not call on a single 
	// threaded application with no other means of being able to call Stop 
	// unless you code in such logic.
	void Run();

	// Stops the IOing system. All work is finished and no more 
	// IOing interactions will be possible afterwards until Reset is called.
	void Stop();

	// Restarts the IOing system after Stop as been called. A new work
	// object is created ad the shutdown flag is cleared.
	void Reset();
};
#endif //UX_IO_HIVE_H_