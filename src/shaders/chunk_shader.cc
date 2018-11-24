#include "shaders/chunk_shader.h"

ChunkShader::ChunkShader() : Shader({ "chunkshader.vert", "chunkshader.frag" }), MVPShader(this), 
	MultiTexShader(this), ClipShader(this), LightShader(this), ShadowShader(this), m_depth(false) {

	bind();
	get_uniform_locations();
}

ChunkShader::ChunkShader(bool depth) 
	: Shader({ "chunkshader.vert", "transparentdepthshader.frag" }), MVPShader(this), 
	MultiTexShader(this), ClipShader(this), LightShader(this), ShadowShader(this), m_depth(depth) {

	bind();
	get_uniform_locations();
}

ChunkShader::~ChunkShader() {

}

void ChunkShader::get_uniform_locations() {
	MVPShader::get_uniform_locations();
	ClipShader::get_uniform_locations();
	MultiTexShader::get_uniform_locations();


	if (!m_depth) {
		LightShader::get_uniform_locations();
		ShadowShader::get_uniform_locations();
	}
}

