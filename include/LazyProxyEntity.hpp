#ifndef LAZY_PROXY_ENTITY_HPP
#define LAZY_PROXY_ENTITY_HPP

#include <array>

namespace detail
{
	template <size_t N>
	class advance_base_impl;
}

template <class ... Iterator>
class LazyProxyEntity
{
	public:

		template <size_t N>
		friend class detail::advance_base_impl;

		using iterator = LazyProxyEntity;
		using const_iterator = const LazyProxyEntity;

	public: // iterators

		iterator begin();
		const_iterator begin() const;
		const_iterator cbegin() const;

		iterator end();
		const_iterator end() const;
		const_iterator cend() const;

		bool operator==(const LazyProxyEntity& rhs) const;
		bool operator!=(const LazyProxyEntity& rhs) const;

		LazyProxyEntity& operator++();

		LazyProxyEntity& operator*();
		const LazyProxyEntity& operator*() const;

		LazyProxyEntity* operator->();
		const LazyProxyEntity* operator->() const;

	public:

		LazyProxyEntity() = default;
		LazyProxyEntity(std::tuple<Iterator...> begins, std::tuple<Iterator...> ends);

		template <class T>
		T& GetComponent();

	private:

		template <size_t N>
		void advance();

		std::tuple<Iterator...> m_begins;
		std::tuple<Iterator...> m_ends;
};

namespace detail
{
	template <size_t N>
	class advance_base_impl
	{
		template <class ... Iterator>
		static void advance_base(LazyProxyEntity<Iterator...>& self);
	};
}

#include "LazyProxyEntity.inl"

#endif // LAZY_PROXY_ENTITY_HPP