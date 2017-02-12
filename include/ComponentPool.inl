#include <algorithm>

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
	return { end_ids(), end_components() };
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::const_iterator ComponentPool<Id, Component>::end() const
{
	return { end_ids(), end_components() };
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
typename ComponentPool<Id, Component>::iterator ComponentPool<Id, Component>::erase(const_iterator pos)
{
	auto distance_deleted_id = std::distance(m_ids.get(), pos.get_id());
	std::move(m_ids.get() + distance_deleted_id + 1, end_ids(), m_ids.get() + distance_deleted_id);
	std::move(m_components.get() + distance_deleted_id + 1, end_components(), m_components.get() + distance_deleted_id);
	m_size -= 1;
	return { m_ids.get() + distance_deleted_id, m_components.get() + distance_deleted_id };;
}

template <class Id, class Component>
bool ComponentPool<Id, Component>::erase(const key_type& key)
{
	auto it = find(key);
	if (it != end())
	{
		erase(it);
		return true;
	}

	return false;
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::iterator ComponentPool<Id, Component>::find(const key_type& id)
{
	auto id_ptr = std::find(m_ids.get(), end_ids(), id);
	return { id_ptr, m_components.get() + std::distance(m_ids.get(), id_ptr) };
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::const_iterator ComponentPool<Id, Component>::find(const key_type& id) const
{
	auto id_ptr = std::find(m_ids.get(), end_ids(), id);
	return { id_ptr, m_components.get() + std::distance(m_ids.get(), id_ptr) };
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::iterator ComponentPool<Id, Component>::insert(key_type&& id, mapped_type&& component)
{
	auto old_size = size();
	auto new_size = old_size + 1;
	reallocate(new_size);

	key_type* new_position = sort_pool(std::forward<key_type>(id));
	auto distance_new_position = std::distance(m_ids.get(), new_position);
	*new_position = std::forward<key_type>(id);
	m_components[distance_new_position] = std::forward<mapped_type>(component);

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
typename ComponentPool<Id, Component>::key_type* ComponentPool<Id, Component>::end_ids() const
{
	return m_ids.get() + size();
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::mapped_type* ComponentPool<Id, Component>::end_components() const
{
	return m_components.get() + size();
}

template <class Id, class Component>
void ComponentPool<Id, Component>::reallocate(size_type new_capacity)
{
	if (new_capacity < capacity())
		return;

	auto m_new_ids = std::make_unique<key_type[]>(new_capacity);
	auto m_new_components = std::make_unique<mapped_type[]>(new_capacity);

	internal::uninitialized_move(m_ids.get(), end_ids(), m_new_ids.get());
	internal::uninitialized_move(m_components.get(), end_components(), m_new_components.get());

	std::swap(m_ids, m_new_ids);
	std::swap(m_components, m_new_components);
	std::swap(m_capacity, new_capacity);
}

template <class Id, class Component>
typename ComponentPool<Id, Component>::key_type* ComponentPool<Id, Component>::sort_pool(key_type&& new_id)
{
	auto id_ptr = std::lower_bound(m_ids.get(), end_ids(), new_id);
	auto distance_new_id = std::distance(m_ids.get(), id_ptr);
	std::move_backward(id_ptr, end_ids(), end_ids() + 1);
	std::move_backward(m_components.get() + distance_new_id, end_components(), end_components() + 1);
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