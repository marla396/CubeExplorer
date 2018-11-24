#include "shaders/postfx/ssao_blur_shader.h"

SSAOBlurShader::SSAOBlurShader() : Shader({ "postfx/ssaoblurshader.comp" }), MultiTexShader(this) {
	bind();
	get_uniform_locations();
}

void SSAOBlurShader::get_uniform_locations() {
	MultiTexShader::get_uniform_locations();
}