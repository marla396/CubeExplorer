#include "shaders/postfx/ssao_merge_shader.h"

SSAOMergeShader::SSAOMergeShader() : Shader({ "postfx/ssaomergeshader.comp" }), MultiTexShader(this) {
	bind();
	get_uniform_locations();
}

void SSAOMergeShader::get_uniform_locations() {
	MultiTexShader::get_uniform_locations();
}

