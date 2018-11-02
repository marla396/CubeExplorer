#include "shaders/water_shader.h"

WaterShader::WaterShader() : Shader({ "watershader.vert", "watershader.tcs", "watershader.tes", "watershader.frag" }), MVPShader(this), MultiTexShader(this) {
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

void WaterShader::upload_light_position(const glm::vec3& position) const {
	upload_uniform(m_light_position_location, position);
}

void WaterShader::get_uniform_locations() {
	MVPShader::get_uniform_locations();
	MultiTexShader::get_uniform_locations();

	m_camera_position_location = get_uniform_location("camera_position");
	m_displacement_factor_location = get_uniform_location("displacement_factor");
	m_water_height_location = get_uniform_location("water_height");
	m_light_position_location = get_uniform_location("light_position");

}