#include "shaders/clip_shader.h"

ClipShader::ClipShader(Shader* parent) : ExtensionShader(parent) {

}

void ClipShader::upload_clip_plane(const glm::vec4& v) const{
	m_parent->upload_uniform(m_clip_plane_location, v);
}


void ClipShader::get_uniform_locations() {
	m_clip_plane_location = m_parent->get_uniform_location("clip_plane");
}