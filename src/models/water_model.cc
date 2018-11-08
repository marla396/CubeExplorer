#include "models/water_model.h"

WaterModel::WaterModel(const glm::vec2& instance_pos) 
	: Quad2DModel(), m_instance_pos(instance_pos) {
	
}

glm::vec2 WaterModel::get_instance_position() const{
	return m_instance_pos;
}

