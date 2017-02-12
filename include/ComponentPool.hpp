#ifndef COMPONENT_POOL_HPP
#define COMPONENT_POOL_HPP

#include <cstddef>
#include <memory>

template <class Id, class Component>
class ComponentPool
{
	private:

		#include "ComponentPoolIterator.hpp"

	public:

		using key_type = Id;
		using mapped_type = Component;
		using value_type = std::pair<const Id, Component>;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = std::pair<const Id, Component>&;
		using const_reference = const std::pair<const Id, Component>&;
		using iterator = ComponentPoolIterator;
		using const_iterator = const ComponentPoolIterator;

	public: // iterators

		iterator begin();
		const_iterator begin() const;
		const_iterator cbegin() const;

		iterator end();
		const_iterator end() const;
		const_iterator cend() const;

	public:

		ComponentPool();
		ComponentPool(ComponentPool&& other);

		size_type capacity() const;
		void clear();

		bool empty() const;
		iterator erase(const_iterator pos);
		bool erase(const key_type& key);

		iterator find(const key_type& id);
		const_iterator find(const key_type& id) const;

		iterator insert(key_type&& id, mapped_type&& component);

		ComponentPool& operator=(ComponentPool&& other);

		size_type size() const;
		void swap(ComponentPool& other);

	private:

		key_type* end_ids() const;
		mapped_type* end_components() const;

		void reallocate(size_type new_capacity);

		key_type* sort_pool(key_type&& id);

	private:

		std::unique_ptr<key_type[]> m_ids;
		std::unique_ptr<mapped_type[]> m_components;
		size_type m_size;
		size_type m_capacity;
};

namespace std
{
	template <class Id, class Component>
	void swap(ComponentPool<Id, Component>& lhs, ComponentPool<Id, Component>& rhs);
}

namespace internal
{
	template<class InputIt, class ForwardIt>
	ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first);
}

#include "ComponentPool.inl"

#endif // COMPONENT_POOL_HPP