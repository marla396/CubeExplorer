#include "shaders/postfx/plain_shader.h"

PlainShader::PlainShader() : Shader({ "postfx/plainshader.vert", "postfx/plainshader.frag" }), TexShader(this) {
	bind();
	get_uniform_locations();
}


void PlainShader::get_uniform_locations() {
	TexShader::get_uniform_locations();
}