#pragma once

#include <string>
#include <array>

#include "extension_shader.h"

template<size_t N>
class MultiTexShader : public ExtensionShader{
public:
	MultiTexShader(Shader* parent) : ExtensionShader(parent) { }
	virtual ~MultiTexShader() { }

	void upload_tex_units(const std::array<int, N>& tex_units) const {

		for (size_t i = 0; i < N; i++) {
			m_parent->upload_uniform(m_tex_unit_locations[i], tex_units[i]);
		}
	}

protected:
	void get_uniform_locations() override {

		for (size_t i = 0; i < N; i++) {
			m_tex_unit_locations[i] = m_parent->get_uniform_location("tex_unit" + std::to_string(i));
		}
	}
private:
	std::array<int, N> m_tex_unit_locations;
};