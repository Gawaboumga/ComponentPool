#ifndef COMPONENT_POOL_HPP
#define COMPONENT_POOL_HPP

#include <cstddef>
#include <memory>
#include <string>

template <class Id, class Component>
class ComponentPool
{
	private:

		#include "ComponentPoolIterator.hpp"

	public:

		using key_type = Id;
		using value_type = Component;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
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

		iterator find(const Id& id);
		const_iterator find(const Id& id) const;

		iterator insert(Id&& id, Component&& component);

		ComponentPool& operator=(ComponentPool&& other);

		size_type size() const;
		void swap(ComponentPool& other);

		std::string toString() const;

	private:

		void reallocate(size_type new_capacity);

		key_type* sort_pool(Id&& id);

	private:

		std::unique_ptr<key_type[]> m_ids;
		std::unique_ptr<value_type[]> m_components;
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