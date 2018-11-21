#include "shaders/mvp_shader.h"

MVPShader::MVPShader(Shader* parent) : ExtensionShader(parent) {
	
}

MVPShader::~MVPShader() {

}

void MVPShader::upload_model_matrix(const glm::mat4& m) const {
	m_parent->upload_uniform(m_model_matrix_location, m);
}

void MVPShader::upload_view_projection_matrix(const glm::mat4& vp) const {
	m_parent->upload_uniform(m_view_projection_matrix_location, vp);
}

void MVPShader::get_uniform_locations() {
	m_model_matrix_location = m_parent->get_uniform_location("model_matrix");
	m_view_projection_matrix_location = m_parent->get_uniform_location("view_projection_matrix");
}