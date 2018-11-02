#include "shaders/inversion_shader.h"

InversionShader::InversionShader() : Shader({ "inversionshader.comp" }), MultiTexShader<3>(this) {
	bind();
	get_uniform_locations();
}

void InversionShader::upload_N(int n) const {
	upload_uniform(m_N_location, n);
}

void InversionShader::upload_pingpong(int pingpong) const {
	upload_uniform(m_pingpong_location, pingpong);
}

void InversionShader::get_uniform_locations() {
	MultiTexShader::get_uniform_locations();

	m_N_location = get_uniform_location("N");
	m_pingpong_location = get_uniform_location("pingpong");
}