#ifndef UTILITY_HPP_
#define UTILITY_HPP_

template<typename F>
boost::thread make_thread(F f)
{
	return boost::thread(f);
}
#endif /* UTILITY_HPP_ */
