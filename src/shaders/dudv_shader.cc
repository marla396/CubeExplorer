#include "shaders/dudv_shader.h"

DuDvShader::DuDvShader() : Shader({ "dudvshader.comp" }), MultiTexShader(this) {
	bind();
	get_uniform_locations();
}

void DuDvShader::get_uniform_locations() {
	MultiTexShader::get_uniform_locations();
}