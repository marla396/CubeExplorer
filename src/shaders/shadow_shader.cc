#include "shaders/shadow_shader.h"

ShadowShader::ShadowShader(Shader* parent) : ExtensionShader(parent){

}

ShadowShader::~ShadowShader() {

}

void ShadowShader::upload_shadow_transform_low(const glm::mat4& transform) const {
	m_parent->upload_uniform(m_shadow_transform_low_location, transform);
}

void ShadowShader::upload_shadow_transform_high(const glm::mat4& transform) const {
	m_parent->upload_uniform(m_shadow_transform_high_location, transform);
}

void ShadowShader::upload_camera_position(const glm::vec3& position) const {
	m_parent->upload_uniform(m_camera_position_location, position);
}

void ShadowShader::get_uniform_locations() {
	m_shadow_transform_low_location = m_parent->get_uniform_location("shadow_transform_low");
	m_shadow_transform_high_location = m_parent->get_uniform_location("shadow_transform_high");
	m_camera_position_location = m_parent->get_uniform_location("camera_position");
}