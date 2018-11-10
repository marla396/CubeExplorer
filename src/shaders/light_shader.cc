#include "shaders/light_shader.h"

LightShader::LightShader(Shader* parent) : ExtensionShader(parent) {

}

LightShader::~LightShader() {

}

void LightShader::upload_light_source(const std::shared_ptr<Light>& light_source) {
	m_parent->upload_uniform(m_light_position_location, light_source->get_position());
	m_parent->upload_uniform(m_light_color_location, light_source->get_color());
}

void LightShader::get_uniform_locations() {
	m_light_position_location = m_parent->get_uniform_location("light_position");
	m_light_color_location = m_parent->get_uniform_location("light_color");
}
