#include "shaders/tex_shader.h"

TexShader::TexShader(Shader* parent) : ExtensionShader(parent) {

}

TexShader::~TexShader() {

}

void TexShader::upload_tex_unit(int i) const {
	m_parent->upload_uniform(m_tex_unit_location, i);
}

void TexShader::get_uniform_locations() {
	m_tex_unit_location = m_parent->get_uniform_location("tex_unit");
}