#include "shaders/postfx/ssao_merge_shader.h"

SSAOMergeShader::SSAOMergeShader() : Shader({ "postfx/ssaomergeshader.comp" }), MultiTexShader(this) {
	bind();
	get_uniform_locations();
}

void SSAOMergeShader::upload_factors(const glm::vec2& factors) const {
	upload_uniform(m_factors_location, factors);
}

void SSAOMergeShader::get_uniform_locations() {
	MultiTexShader::get_uniform_locations();

	m_factors_location = get_uniform_location("factors");
}

