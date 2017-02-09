#include <algorithm>
#include <utility>

// Iterators

template <class Id, class Component>
typename ComponentPool<Id, Component>::iterator ComponentPool<Id, Component>::begin()
{
	return { m_ids.get(), m_components.get() };
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::const_iterator ComponentPool<Id, Component>::begin() const
{
	return { m_ids.get(), m_components.get() };
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::const_iterator ComponentPool<Id, Component>::cbegin() const
{
	return begin();
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::iterator ComponentPool<Id, Component>::end()
{
	return { m_ids.get() + size(), m_components.get() + size() };
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::const_iterator ComponentPool<Id, Component>::end() const
{
	return { m_ids.get() + size(), m_components.get() + size() };
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::const_iterator ComponentPool<Id, Component>::cend() const
{
	return end();
}

template <class Id, class Component>
ComponentPool<Id, Component>::ComponentPool() :
	m_ids{ nullptr },
	m_components{ nullptr },
	m_size{},
	m_capacity{}
{
}

template <class Id, class Component>
ComponentPool<Id, Component>::ComponentPool(ComponentPool&& other) :
	m_ids{ std::move(other.m_ids) },
	m_components{ std::move(other.m_components) },
	m_size{ std::exchange(other.m_size, {}) },
	m_capacity{ std::exchange(other.m_capacity, {}) }
{
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::size_type ComponentPool<Id, Component>::capacity() const
{
	return m_capacity;
}

template <class Id, class Component>
void ComponentPool<Id, Component>::clear()
{
	m_size = 0;
}

template <class Id, class Component>
bool ComponentPool<Id, Component>::empty() const
{
	return size() == 0;
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::iterator ComponentPool<Id, Component>::find(const Id& id)
{
	auto id_ptr = std::find(m_ids.get(), m_ids.get() + size(), id);
	return { id_ptr, m_components.get() + std::distance(m_ids.get(), id_ptr) };
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::const_iterator ComponentPool<Id, Component>::find(const Id& id) const
{
	auto id_ptr = std::find(m_ids.get(), m_ids.get() + size(), id);
	return { id_ptr, m_components.get() + std::distance(m_ids.get(), id_ptr) };
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::iterator ComponentPool<Id, Component>::insert(Id&& id, Component&& component)
{
	auto old_size = size();
	auto new_size = old_size + 1;
	reallocate(new_size);

	key_type* new_position = sort_pool(std::forward<Id>(id));
	auto distance_new_position = std::distance(m_ids.get(), new_position);
	*new_position = std::forward<Id>(id);
	m_components[distance_new_position] = std::forward<Component>(component);

	m_size = new_size;

	return { new_position, m_components.get() + distance_new_position };
}

template <class Id, class Component>
ComponentPool<Id, Component>& ComponentPool<Id, Component>::operator=(ComponentPool&& other)
{
	m_ids = std::move(other.m_ids);
	m_components = std::move(other.m_components);
	m_size = std::exchange(other.m_size, {});
	m_capacity = std::exchange(other.m_capacity, {});

	return *this;
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::size_type ComponentPool<Id, Component>::size() const
{
	return m_size;
}

template <class Id, class Component>
void ComponentPool<Id, Component>::swap(ComponentPool& other)
{
	std::swap(m_ids, other.m_ids);
	std::swap(m_components, other.m_components);
	std::swap(m_size, other.m_size);
	std::swap(m_capacity, other.m_capacity);
}

template <class Id, class Component>
void ComponentPool<Id, Component>::reallocate(size_type new_capacity)
{
	if (new_capacity < capacity())
		return;

	auto m_new_ids = std::make_unique<key_type[]>(new_capacity);
	auto m_new_components = std::make_unique<value_type[]>(new_capacity);

	internal::uninitialized_move(m_ids.get(), m_ids.get() + size(), m_new_ids.get());
	internal::uninitialized_move(m_components.get(), m_components.get() + size(), m_new_components.get());

	std::swap(m_ids, m_new_ids);
	std::swap(m_components, m_new_components);
	std::swap(m_capacity, new_capacity);
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::key_type* ComponentPool<Id, Component>::sort_pool(Id&& new_id)
{
	auto id_ptr = std::lower_bound(m_ids.get(), m_ids.get() + size(), new_id);
	auto distance_new_id = std::distance(m_ids.get(), id_ptr);
	std::move_backward(id_ptr, m_ids.get() + size(), m_ids.get() + size() + 1);
	std::move_backward(m_components.get() + distance_new_id, m_components.get() + size(), m_components.get() + size() + 1);
	return id_ptr;
}

namespace std
{
	template <class Id, class Component>
	void swap(ComponentPool<Id, Component>& lhs, ComponentPool<Id, Component>& rhs)
	{
		return lhs.swap(rhs);
	}
}

namespace internal
{
	template<class InputIt, class ForwardIt>
	ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first)
	{
		typedef typename std::iterator_traits<ForwardIt>::value_type Value;
		ForwardIt current = d_first;
		try {
			for (; first != last; ++first, (void) ++current) {
				::new (static_cast<void*>(std::addressof(*current))) Value(std::move(*first));
			}
			return current;
		} catch (...) {
			for (; d_first != current; ++d_first) {
				d_first->~Value();
			}
			throw;
		}
	}
}