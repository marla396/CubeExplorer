#include "shaders/butterfly_shader.h"

ButterflyShader::ButterflyShader() : Shader({ "butterflyshader.comp" }), MultiTexShader(this) {
	bind();
	get_uniform_locations();
}

void ButterflyShader::upload_stage(int stage) const {
	upload_uniform(m_stage_location, stage);
}

void ButterflyShader::upload_pingpong(int pingpong) const {
	upload_uniform(m_pingpong_location, pingpong);
}

void ButterflyShader::upload_direction(int direction) const {
	upload_uniform(m_direction_location, direction);
}

void ButterflyShader::get_uniform_locations() {
	MultiTexShader::get_uniform_locations();

	m_stage_location = get_uniform_location("stage");
	m_pingpong_location = get_uniform_location("pingpong");
	m_direction_location = get_uniform_location("direction");
}

