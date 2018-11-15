#include "shaders/entity_shader.h"

EntityShader::EntityShader() : Shader{ "entityshader.vert", "entityshader.frag" }, MVPShader(this), MultiTexShader(this), LightShader(this), ClipShader(this), ShadowShader(this), m_depth(false) {
	bind();
	get_uniform_locations();
}

EntityShader::EntityShader(bool depth) : Shader{ "entityshader.vert", "depthshader.frag" }, MVPShader(this), MultiTexShader(this), LightShader(this), ClipShader(this), ShadowShader(this), m_depth(depth) {
	bind();
	get_uniform_locations();
}

void EntityShader::get_uniform_locations() {

	MVPShader::get_uniform_locations();
	ClipShader::get_uniform_locations();

	if (!m_depth) {
		ShadowShader::get_uniform_locations();
		MultiTexShader::get_uniform_locations();
		LightShader::get_uniform_locations();
	}
}