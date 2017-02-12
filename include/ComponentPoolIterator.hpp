#ifndef COMPONENT_POOL_ITERATOR_HPP
#define COMPONENT_POOL_ITERATOR_HPP

class ComponentPoolIterator : std::forward_iterator_tag
{
	public:

		ComponentPoolIterator() = default;
		ComponentPoolIterator(Id* id, Component* component) :
			m_id{ id },
			m_component{ component }
		{
		}

		bool operator==(const ComponentPoolIterator& rhs) const
		{
			return m_id == rhs.m_id;
		}

		bool operator!=(const ComponentPoolIterator& rhs) const
		{
			return m_id != rhs.m_id;
		}

		Component& operator*()
		{
			return *m_component;
		}

		const Component& operator*() const
		{
			return *m_component;
		}

		Component* operator->()
		{
			return m_component;
		}

		const Component* operator->() const
		{
			return m_component;
		}

		ComponentPoolIterator& operator++()
		{
			++m_id;
			++m_component;
			return *this;
		}

		ComponentPoolIterator operator++(int)
		{
			auto copy = *this;
			++m_id;
			++m_component;
			return copy;
		}

		Id* get_id() const
		{
			return m_id;
		}

	private:

		Id* m_id;
		Component* m_component;
};

#endif // COMPONENT_POOL_ITERATOR_HPP