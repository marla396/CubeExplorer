#pragma once

#include <glm/glm.hpp>

#include "camera.h"

class Light {
public:
	Light(const glm::vec3& position, const glm::vec3& color);

	glm::vec3 get_position() const;
	void set_position(const glm::vec3& position);

	glm::vec3 get_color() const;
	void set_color(const glm::vec3& color);

	glm::mat4 get_view_matrix() const;
	glm::mat4 get_projection_matrix() const;

	glm::mat4 get_transform_matrix() const;
private:
	glm::vec3 m_position;
	glm::vec3 m_color;
};