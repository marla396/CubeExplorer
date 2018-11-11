#include "world/light.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/matrix.hpp>

#include "world/world_const.h"


Light::Light(const glm::vec3& position, const glm::vec3& color) 
	: m_position(position), m_color(color){

}

glm::vec3 Light::get_position() const {
	return m_position;
}

void Light::set_position(const glm::vec3& position) {
	m_position = position;
}

glm::vec3 Light::get_color() const {
	return m_color;
}

void Light::set_color(const glm::vec3& color) {
	m_color = color;
}

glm::mat4 Light::get_view_matrix(const Camera& camera) const {
	return glm::lookAt(m_position, WORLD_CENTER, { 0.0f, 1.0f, 0.0f });
}

glm::mat4 Light::get_projection_matrix(const Camera& camera) const {

	float diagnoal_size = 1.1f * std::sqrt(2.0f) * static_cast<float>(CHUNK_SIZE * WORLD_SIZE);

	return glm::ortho(-diagnoal_size / 2.0f, diagnoal_size / 2.0f, -diagnoal_size / 2.0f, diagnoal_size / 2.0f, 0.1f, diagnoal_size);
}

glm::mat4 Light::get_transform_matrix(const Camera& camera) const {
	return get_projection_matrix(camera) * get_view_matrix(camera);
}