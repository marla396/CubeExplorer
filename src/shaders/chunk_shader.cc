#include "shaders/chunk_shader.h"

ChunkShader::ChunkShader() : Shader({ "chunkshader.vert", "chunkshader.frag" }), MVPShader(this), TexShader(this), ClipShader(this) {
	bind();
	get_uniform_locations();
}

ChunkShader::~ChunkShader() {

}

void ChunkShader::get_uniform_locations() {
	MVPShader::get_uniform_locations();
	TexShader::get_uniform_locations();
	ClipShader::get_uniform_locations();
}

