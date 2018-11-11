#include "shaders/chunk_shader.h"

ChunkShader::ChunkShader() : Shader({ "chunkshader.vert", "chunkshader.frag" }), MVPShader(this), MultiTexShader(this), ClipShader(this), LightShader(this) {
	bind();
	get_uniform_locations();
}

ChunkShader::ChunkShader(bool depth) 
	: Shader({ "chunkshader.vert", "depthshader.frag" }), MVPShader(this), MultiTexShader(this), ClipShader(this), m_depth(depth), LightShader(this) {
	bind();
	get_uniform_locations();
}

ChunkShader::~ChunkShader() {

}

void ChunkShader::upload_shadow_transform(const glm::mat4& transform) const {
	upload_uniform(m_shadow_transform_location, transform);
}

void ChunkShader::get_uniform_locations() {
	MVPShader::get_uniform_locations();
	ClipShader::get_uniform_locations();
	LightShader::get_uniform_locations();

	if (!m_depth) {
		MultiTexShader::get_uniform_locations();
		m_shadow_transform_location = get_uniform_location("shadow_transform");
	}
}

