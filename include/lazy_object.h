#pragma once

#include <functional>

template<typename T>
class LazyObject {
public:
	LazyObject(const std::function<T()>& f) : m_update_function(f), m_dirty(true) {

	}

	T get() {
		if (m_dirty) {
			m_object = m_update_function();
			m_dirty = false;
		}
		return m_object;
	}

	void notify() {
		m_dirty = true;
	}
private:
	T m_object;
	std::function<T()> m_update_function;
	bool m_dirty;
};