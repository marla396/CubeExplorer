#pragma once

#include <functional>

template<typename T>
class LazyObject {
public:
	LazyObject(const std::function<T()>& f) : m_update_function(f), m_dirty(true) {

	}

	T get() const{
		if (m_dirty) {
			m_object = m_update_function();
			m_dirty = false;
		}
		return m_object;
	}

	void set(const T& obj) {
		m_object = obj;
		m_dirty = false;
	}

	void notify() const{
		m_dirty = true;
	}

	void set_update_function(const std::function<T()>& f) {
		m_update_function = f;
		m_dirty = true;
	}
private:
	mutable T m_object;
	std::function<T()> m_update_function;
	mutable bool m_dirty;
};