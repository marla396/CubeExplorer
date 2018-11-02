#include "shaders/postfx/lowpass_x_shader.h"

LowpassXShader::LowpassXShader() : Shader({ "postfx/plainshader.vert", "postfx/lowpassxshader.frag" }), TexShader(this) {
	get_uniform_locations();
}

void LowpassXShader::upload_kernel(const std::vector<float>& kernel) const {
	upload_uniform(m_radius_location, static_cast<int>(kernel.size()));
	upload_uniform(m_kernel_location, kernel.data(), kernel.size());
}

void LowpassXShader::get_uniform_locations() {
	TexShader::get_uniform_locations();

	m_radius_location = get_uniform_location("radius");
	m_kernel_location = get_uniform_location("kernel");
}