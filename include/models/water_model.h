#pragma once

#include "quad2d_model.h"

#include <glm/glm.hpp>

class WaterModel : public Quad2DModel{
public:
	WaterModel(const glm::vec2& instance_pos);

	glm::vec2 get_instance_position() const;
private:
	glm::vec2 m_instance_pos;
};
