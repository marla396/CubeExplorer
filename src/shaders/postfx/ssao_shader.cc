#include "shaders/postfx/ssao_shader.h"

SSAOShader::SSAOShader() : Shader({ "postfx/ssaoshader.vert", "postfx/ssaoshader.frag" }), TexShader(this) {
	bind();
	get_uniform_locations();
}

void SSAOShader::upload_kernel(const std::vector<glm::vec3>& kernel) const {
	upload_uniform(m_kernel_location, kernel);
}

void SSAOShader::upload_projection_depth(const glm::vec2& depth) const {
	upload_uniform(m_projection_depth_location, depth);
}


void SSAOShader::get_uniform_locations() {
	TexShader::get_uniform_locations();

	m_kernel_location = get_uniform_location("kernel");
	m_projection_depth_location = get_uniform_location("projection_depth");
}