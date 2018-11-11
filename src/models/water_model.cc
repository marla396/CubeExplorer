#include "models/water_model.h"

WaterModel::WaterModel(const glm::vec2& instance_pos) 
	: Quad2DModel(), m_instance_pos(instance_pos) {
	
}

glm::vec2 WaterModel::get_instance_position() const{
	return m_instance_pos;
}

glm::vec3 WaterModel::get_center() const {
	return m_position + m_scale / 2.0f;
}

float WaterModel::get_bounding_radius() const {
	return std::sqrt(2.0f) * m_scale.x;
}