#pragma once

#include <glm/glm.hpp>
#include <array>

#include "camera.h"
#include "world_const.h"

class Light {
public:
	Light(const glm::vec3& position, const glm::vec3& color);

	void update(Camera& camera, float time);

	glm::vec3 get_position() const;	
	void set_position(const glm::vec3& position);

	glm::vec3 get_color() const;
	void set_color(const glm::vec3& color);

	glm::mat4 get_view_matrix() const;
	glm::mat4 get_projection_matrix(int cascade) const;
	glm::mat4 get_transform_matrix(int cascade) const;
	float get_shadow_cascade_end(int cascade) const;



private:

	std::array<glm::vec2, SHADOW_CASCADES> m_shadow_cascades;
	glm::mat4 m_view_matrix;
	std::array<glm::mat4, SHADOW_CASCADES> m_projection_matrices;
	std::array<glm::mat4, SHADOW_CASCADES> m_transform_matrices;

	glm::vec3 m_position;
	glm::vec3 m_color;
};