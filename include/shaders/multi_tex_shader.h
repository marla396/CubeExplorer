#pragma once

#include <string>
#include <array>

#include "extension_shader.h"

template<size_t N>
class MultiTexShader : public ExtensionShader{
public:
	MultiTexShader(Shader* parent) : ExtensionShader(parent) { }
	virtual ~MultiTexShader() { }

	void upload_tex_units(const std::vector<int>& tex_units) const {

		for (size_t i = 0; i < tex_units.size(); i++) {
			m_parent->upload_uniform(m_tex_unit_locations[i], tex_units[i]);
		}
	}

protected:
	void get_uniform_locations(size_t n = N)  {

		for (size_t i = 0; i < n; i++) {
			m_tex_unit_locations[i] = m_parent->get_uniform_location("tex_unit" + std::to_string(i));
		}
	}
private:
	std::array<int, N> m_tex_unit_locations;
};