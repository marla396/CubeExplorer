#include "shaders/entity_shader.h"

EntityShader::EntityShader() : Shader{ "entityshader.vert", "entityshader.frag" }, MVPShader(this), MultiTexShader(this), LightShader(this), m_depth(false) {
	bind();
	get_uniform_locations();
}

EntityShader::EntityShader(bool depth) : Shader{ "entityshader.vert", "depthshader.frag" }, MVPShader(this), MultiTexShader(this), LightShader(this), m_depth(depth) {
	bind();
	get_uniform_locations();
}

void EntityShader::upload_shadow_transform(const glm::mat4& transform) const {
	upload_uniform(m_shadow_transform_location, transform);
}

void EntityShader::get_uniform_locations() {

	MVPShader::get_uniform_locations();

	if (!m_depth) {
		m_shadow_transform_location = get_uniform_location("shadow_transform");
		MultiTexShader::get_uniform_locations();
		LightShader::get_uniform_locations();
	}
}