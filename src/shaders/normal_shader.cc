#include "shaders/normal_shader.h"

NormalShader::NormalShader() : Shader({ "normalshader.comp" }), MultiTexShader(this) {
	bind();
	get_uniform_locations();
}

void NormalShader::upload_N(int n) const {
	upload_uniform(m_N_location, n);
}

void NormalShader::upload_strength(float strength) const {
	upload_uniform(m_strength_location, strength);

}

void NormalShader::get_uniform_locations() {
	MultiTexShader::get_uniform_locations();

	m_N_location = get_uniform_location("N");
	m_strength_location = get_uniform_location("normal_strength");
}