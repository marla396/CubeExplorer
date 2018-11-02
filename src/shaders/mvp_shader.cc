#include "shaders/mvp_shader.h"

MVPShader::MVPShader(Shader* parent) : ExtensionShader(parent) {
	
}

MVPShader::~MVPShader() {

}

void MVPShader::upload_model_matrix(const glm::mat4& m) const {
	m_parent->upload_uniform(m_model_matrix_location, m);
}

void MVPShader::upload_view_matrix(const glm::mat4& v) const {
	m_parent->upload_uniform(m_view_matrix_location, v);
}

void MVPShader::upload_projection_matrix(const glm::mat4& p) const {
	m_parent->upload_uniform(m_projection_matrix_location, p);
}

void MVPShader::get_uniform_locations() {
	m_model_matrix_location = m_parent->get_uniform_location("model_matrix");
	m_view_matrix_location = m_parent->get_uniform_location("view_matrix");
	m_projection_matrix_location = m_parent->get_uniform_location("projection_matrix");
}