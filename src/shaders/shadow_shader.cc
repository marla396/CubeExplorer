#include <string>

#include "shaders/shadow_shader.h"


ShadowShader::ShadowShader(Shader* parent) : ExtensionShader(parent){

}

ShadowShader::~ShadowShader() {

}

void ShadowShader::upload_shadow_maps(int first) const {
	for (int i = 0; i < SHADOW_CASCADES; i++) {
		m_parent->upload_uniform(m_shadow_map_locations[i], first + i);
	}
}

void ShadowShader::upload_shadow_transforms(const std::shared_ptr<Light>& light) const {
	for (int i = 0; i < SHADOW_CASCADES; i++) {
		m_parent->upload_uniform(m_shadow_transform_locations[i], light->get_transform_matrix(i));
	}
}

void ShadowShader::upload_shadow_cascade_end(const Camera& camera, const std::shared_ptr<Light>& light) const {
	for (int i = 0; i < SHADOW_CASCADES; i++) {
		m_parent->upload_uniform(m_shadow_cascade_end_locations[i], light->get_shadow_cascade_end(i) / camera.get_far());
	}
}

void ShadowShader::upload_view_matrix(const glm::mat4& matrix) const {
	m_parent->upload_uniform(m_view_matrix_location, matrix);
}

void ShadowShader::upload_projection_depth(const glm::vec2& projection_depth) const {
	m_parent->upload_uniform(m_projection_depth_location, projection_depth);
}

void ShadowShader::get_uniform_locations() {
	for (int i = 0; i < SHADOW_CASCADES; i++) {
		m_shadow_transform_locations[i] = m_parent->get_uniform_location("shadow_transforms[" + std::to_string(i) + "]");
		m_shadow_map_locations[i] = m_parent->get_uniform_location("shadow_maps[" + std::to_string(i) + "]");
		m_shadow_cascade_end_locations[i] = m_parent->get_uniform_location("shadow_cascade_end[" + std::to_string(i) + "]");
	}

	m_view_matrix_location = m_parent->get_uniform_location("view_matrix");
	m_projection_depth_location = m_parent->get_uniform_location("projection_depth");
}