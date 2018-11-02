#include "shaders/h0k_shader.h"

H0kShader::H0kShader() : Shader({ "h0kshader.comp" }), MultiTexShader<6>(this) {
	bind();
	get_uniform_locations();
}

void H0kShader::upload_N(int n) const {
	upload_uniform(m_N_location, n);
}

void H0kShader::upload_L(int l) const {
	upload_uniform(m_L_location, l);
}

void H0kShader::upload_amplitude(float a) const {
	upload_uniform(m_amplitude_location, a);
}

void H0kShader::upload_wind_speed(float a) const {
	upload_uniform(m_wind_speed_location, a);
}

void H0kShader::upload_wind_direction(const glm::vec2 dir) const {
	upload_uniform(m_wind_direction_location, dir);
}

void H0kShader::upload_capillar_supress_factor(float c) const {
	upload_uniform(m_capillar_supress_factor_location, c);
}

void H0kShader::get_uniform_locations() {
	MultiTexShader::get_uniform_locations();

	m_N_location = get_uniform_location("N");
	m_L_location = get_uniform_location("L");
	m_amplitude_location = get_uniform_location("amplitude");
	m_wind_speed_location = get_uniform_location("wind_speed");
	m_wind_direction_location = get_uniform_location("wind_direction");
	m_capillar_supress_factor_location = get_uniform_location("capillar_supress_factor");
}
