#ifndef UX_UTIL_UTILITY_HPP_
#define UX_UTIL_UTILITY_HPP_

template<typename F>
boost::thread make_thread(F f)
{
	return boost::thread(f);
}
#endif /* UX_UTIL_UTILITY_HPP_ */
