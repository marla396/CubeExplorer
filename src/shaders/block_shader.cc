#include "shaders/block_shader.h"

BlockShader::BlockShader() : Shader({ "blockshader.vert", "blockshader.frag" } ), MVPShader(this), TexShader(this) {
	bind();
	get_uniform_locations();
}

BlockShader::~BlockShader() {

}

void BlockShader::get_uniform_locations() {
	MVPShader::get_uniform_locations();
	TexShader::get_uniform_locations();
}

