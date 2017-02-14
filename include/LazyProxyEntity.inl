#include "ComponentPool.hpp"
#include <tuple>

// iterators

template <class ... Iterator>
typename LazyProxyEntity<Iterator...>::iterator LazyProxyEntity<Iterator...>::begin()
{
	detail::advance_base_impl<sizeof...(Iterator)>::advance_base(*this);
	return *this;
}

template <class ... Iterator>
typename LazyProxyEntity<Iterator...>::const_iterator LazyProxyEntity<Iterator...>::begin() const
{
	detail::advance_base_impl<sizeof...(Iterator)>::advance_base(*this);
	return *this;
}

template <class ... Iterator>
typename LazyProxyEntity<Iterator...>::const_iterator LazyProxyEntity<Iterator...>::cbegin() const
{
	return begin();
}

template <class ... Iterator>
typename LazyProxyEntity<Iterator...>::iterator LazyProxyEntity<Iterator...>::end()
{
	return { m_ends, m_ends };
}

template <class ... Iterator>
typename LazyProxyEntity<Iterator...>::const_iterator LazyProxyEntity<Iterator...>::end() const
{
	return { m_ends, m_ends };
}

template <class ... Iterator>
typename LazyProxyEntity<Iterator...>::const_iterator LazyProxyEntity<Iterator...>::cend() const
{
	return end();
}

template <class ... Iterator>
bool LazyProxyEntity<Iterator...>::operator==(const LazyProxyEntity& rhs) const
{
	return one_equal_tuples(m_begins, rhs.m_begins);
}

template <class ... Iterator>
bool LazyProxyEntity<Iterator...>::operator!=(const LazyProxyEntity& rhs) const
{
	return !operator==(rhs);
}

template <class ... Iterator>
LazyProxyEntity<Iterator...>& LazyProxyEntity<Iterator...>::operator++()
{
	advance<sizeof...(Iterator)>();
	return *this;
}

template <class ... Iterator>
LazyProxyEntity<Iterator...>& LazyProxyEntity<Iterator...>::operator*()
{
	return *this;
}

template <class ... Iterator>
const LazyProxyEntity<Iterator...>& LazyProxyEntity<Iterator...>::operator*() const
{
	return *this;
}

template <class ... Iterator>
LazyProxyEntity<Iterator...>* LazyProxyEntity<Iterator...>::operator->()
{
	return this;
}

template <class ... Iterator>
const LazyProxyEntity<Iterator...>* LazyProxyEntity<Iterator...>::operator->() const
{
	return this;
}

template <class ... Iterator>
LazyProxyEntity<Iterator...>::LazyProxyEntity(std::tuple<Iterator...> begins, std::tuple<Iterator...> ends) :
	m_begins{ begins },
	m_ends{ ends }
{
}

template <class ... Iterator>
template <class T>
T& LazyProxyEntity<Iterator...>::GetComponent()
{
	using id_type = std::common_type_t<typename Iterator::key_type...>;
	using iterator_type = decltype(std::declval<ComponentPool<id_type, T>>().begin());
	return *std::get<iterator_type>(m_begins);
}

template <class ... Iterator>
template <size_t N>
void LazyProxyEntity<Iterator...>::advance()
{
	// We are currently on same elements
	for_each(m_begins, [](auto& it) {
		++it;
	});

	detail::advance_base_impl<N>::advance_base(*this);
}

namespace detail
{
	template <>
	class advance_base_impl<1>
	{
		public:

			template <class ... Iterator>
			static void advance_base(LazyProxyEntity<Iterator...>&)
			{
				// Since we do it in advance !
			}
	};

	template <>
	class advance_base_impl<2>
	{
		public:

			template <class ... Iterator>
			static void advance_base(LazyProxyEntity<Iterator...>& self)
			{
				while (std::get<0>(self.m_begins) != std::get<0>(self.m_ends) && std::get<1>(self.m_begins) != std::get<1>(self.m_ends))
				{
					if (*std::get<0>(self.m_begins).get_id() < *std::get<1>(self.m_begins).get_id())
						++std::get<0>(self.m_begins);
					else
					{
						if (*std::get<1>(self.m_begins).get_id() < *std::get<0>(self.m_begins).get_id())
							++std::get<1>(self.m_begins);

						if (*std::get<0>(self.m_begins).get_id() == *std::get<1>(self.m_begins).get_id())
							return;
					}
				}
			}
	};

	template <>
	class advance_base_impl<3>
	{
		public:

			template <class ... Iterator>
			static void advance_base(LazyProxyEntity<Iterator...>& self)
			{
				while (std::get<0>(self.m_begins) != std::get<0>(self.m_ends) && std::get<1>(self.m_begins) != std::get<1>(self.m_ends) && std::get<2>(self.m_begins) != std::get<2>(self.m_ends))
				{
					if (*std::get<0>(self.m_begins).get_id() == *std::get<1>(self.m_begins).get_id() &&
						*std::get<0>(self.m_begins).get_id() == *std::get<2>(self.m_begins).get_id())
						return;

					if (*std::get<0>(self.m_begins).get_id() < *std::get<1>(self.m_begins).get_id())
						++std::get<0>(self.m_begins);
					else
					{
						if (*std::get<1>(self.m_begins).get_id() < *std::get<2>(self.m_begins).get_id())
							++std::get<1>(self.m_begins);
						else
							++std::get<2>(self.m_begins);
					}
				}
			}
	};
}

//! Stolen from http://codereview.stackexchange.com/questions/30846/zip-like-functionality-with-c11s-range-based-for-loop

template <size_t... n>
struct ct_integers_list {
    template <size_t m>
    struct push_back
    {
        typedef ct_integers_list<n..., m> type;
    };
};

template <size_t max>
struct ct_iota_1
{
    typedef typename ct_iota_1<max-1>::type::template push_back<max>::type type;
};

template <>
struct ct_iota_1<0>
{
    typedef ct_integers_list<> type;
};

/***************************
// return a subset of a tuple
***************************/
template <size_t... indices, typename Tuple>
auto tuple_subset(const Tuple& tpl, ct_integers_list<indices...>)
    -> decltype(std::make_tuple(std::get<indices>(tpl)...))
{
    return std::make_tuple(std::get<indices>(tpl)...);
    // this means:
    //   make_tuple(get<indices[0]>(tpl), get<indices[1]>(tpl), ...)
}

/***************************
// return the tail of a tuple
***************************/
template <typename Head, typename... Tail>
inline std::tuple<Tail...> tuple_tail(const std::tuple<Head, Tail...>& tpl)
{
    return tuple_subset(tpl, typename ct_iota_1<sizeof...(Tail)>::type());
    // this means:
    //   tuple_subset<1, 2, 3, ..., sizeof...(Tail)-1>(tpl, ..)
}

template<typename T1>
inline bool one_equal_tuples( const std::tuple<T1>& t1,  const std::tuple<T1>& t2 )
{
	return (std::get<0>(t1) == std::get<0>(t2));
}

template<typename T1, typename... Ts>
inline bool one_equal_tuples( const std::tuple<T1, Ts...>& t1,  const std::tuple<T1, Ts...>& t2 )
{
	return (std::get<0>(t1) == std::get<0>(t2)) || one_equal_tuples( tuple_tail(t1), tuple_tail(t2) );
}

template <typename Tuple, typename F, std::size_t ...Indices>
void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>)
{
    using swallow = int[];
    (void)swallow{1,
        (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...
    };
}

template <typename Tuple, typename F>
void for_each(Tuple&& tuple, F&& f)
{
    constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
    for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                  std::make_index_sequence<N>{});
}