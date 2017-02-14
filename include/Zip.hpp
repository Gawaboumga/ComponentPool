#ifndef ZIP_HPP
#define ZIP_HPP

#include "LazyProxyEntity.hpp"

template <class ... Collections>
LazyProxyEntity<typename Collections::iterator...> zip(const Collections&... pools)
{
	return { std::make_tuple(pools.begin()...), std::make_tuple(pools.end()...) };
}

#endif // ZIP_HPP