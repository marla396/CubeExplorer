#include "shaders/postfx/ssao_shader.h"

SSAOShader::SSAOShader() : Shader({ "postfx/ssaoshader.vert", "postfx/ssaoshader.frag" }), MultiTexShader(this) {
	bind();
	get_uniform_locations();
}

void SSAOShader::upload_kernel(const std::vector<glm::vec3>& kernel) const {
	upload_uniform(m_kernel_location, kernel);
}

void SSAOShader::upload_resolution(const glm::vec2& res) const {
	upload_uniform(m_resolution_location, res);
}

void SSAOShader::upload_projection_matrix(const glm::mat4& projection) const {
	upload_uniform(m_projection_matrix_location, projection);
}

void SSAOShader::get_uniform_locations() {
	MultiTexShader::get_uniform_locations();

	m_kernel_location = get_uniform_location("kernel");
	m_resolution_location = get_uniform_location("resolution");
	m_projection_matrix_location = get_uniform_location("projection_matrix");
}