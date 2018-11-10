#include "shaders/water_shader.h"

WaterShader::WaterShader() : Shader({ "watershader.vert", "watershader.tcs", "watershader.tes", "watershader.frag" }), MVPShader(this), MultiTexShader(this), LightShader(this), m_depth(false) {
	bind();
	get_uniform_locations();
}

WaterShader::WaterShader(bool depth) : 
	Shader({ "watershader.vert", "watershader.tcs", "watershader.tes", "depthshader.frag" }), MVPShader(this), MultiTexShader(this), LightShader(this), m_depth(depth) {

	bind();
	get_uniform_locations();
}

WaterShader::~WaterShader() {

}

void WaterShader::upload_camera_position(const glm::vec3& pos) const {
	upload_uniform(m_camera_position_location, pos);
}

void WaterShader::upload_displacement_factor(float factor) const {
	upload_uniform(m_displacement_factor_location, factor);
}

void WaterShader::upload_water_height(float height) const {
	upload_uniform(m_water_height_location, height);
}

void WaterShader::upload_quad_dimension(int dim) const{
	upload_uniform(m_quad_dimension_location, dim);
}

void WaterShader::upload_quad_instance(const glm::vec2& instance) const{
	upload_uniform(m_quad_instance_location, instance);
}

void WaterShader::upload_shadow_transform(const glm::mat4& transform) const {
	upload_uniform(m_shadow_transform_location, transform);
}

void WaterShader::get_uniform_locations() {
	MVPShader::get_uniform_locations();
	MultiTexShader::get_uniform_locations();

	if (!m_depth) {
		LightShader::get_uniform_locations();
		m_shadow_transform_location = get_uniform_location("shadow_transform");
	}

	m_camera_position_location = get_uniform_location("camera_position");
	m_displacement_factor_location = get_uniform_location("displacement_factor");
	m_water_height_location = get_uniform_location("water_height");
	m_quad_dimension_location = get_uniform_location("quad_dimension");
	m_quad_instance_location = get_uniform_location("quad_instance");
}
