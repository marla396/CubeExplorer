#include "shaders/hkt_shader.h"
HktShader::HktShader() : Shader({ "hktshader.comp" }), MultiTexShader<5>(this) {
	bind();
	get_uniform_locations();
}

void HktShader::upload_N(int n) const {
	upload_uniform(m_N_location, n);
}

void HktShader::upload_L(int l) const {
	upload_uniform(m_L_location, l);
}

void HktShader::upload_time(float time) const {
	upload_uniform(m_time_location, time);
}

void HktShader::get_uniform_locations() {
	MultiTexShader::get_uniform_locations();

	m_N_location = get_uniform_location("N");
	m_L_location = get_uniform_location("L");
	m_time_location = get_uniform_location("t");
}
