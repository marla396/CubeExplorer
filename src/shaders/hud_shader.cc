#include "shaders/hud_shader.h"

HUDShader::HUDShader() : Shader({ "hudshader.vert", "hudshader.frag" }), TexShader(this) {
	bind();
	get_uniform_locations();
}

HUDShader::~HUDShader() {

}

void HUDShader::upload_transformation_matrix(const glm::mat4& m) const {
	upload_uniform(m_transformation_matrix_location, m);
}

void HUDShader::get_uniform_locations() {
	TexShader::get_uniform_locations();
	m_transformation_matrix_location = get_uniform_location("transformation_matrix");
}