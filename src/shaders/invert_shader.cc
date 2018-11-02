#include "shaders/invert_shader.h"

InvertShader::InvertShader() : Shader({ "invertshader.comp" }), MultiTexShader<2>(this) {
	bind();
	get_uniform_locations();
}

void InvertShader::get_uniform_locations() {
	MultiTexShader::get_uniform_locations();
}
