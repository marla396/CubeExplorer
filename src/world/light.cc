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

glm::mat4 Light::get_view_matrix(const Camera& camera, ShadowMapQuality quality) const {

	if (quality == LOW) {
		return glm::lookAt(m_position, WORLD_CENTER, { 0.0f, 1.0f, 0.0f });
	}
	else if (quality == HIGH) {
		glm::vec3 n = glm::normalize(m_position); //normalize sun vector

		glm::vec3 pos = LIGHT_HIGH_DISTANCE * glm::dot(n, camera.get_position()) * camera.get_position(); //scale sun vector
	
//		pos += camera.get_position(); // move it in parallel towards the camera


		return glm::lookAt(m_player_position, camera.get_position(), { 0.0f, 1.0f, 0.0f });
	}
}

glm::mat4 Light::get_projection_matrix(ShadowMapQuality quality) const {

	float diagonal_size;
	
	if (quality == LOW) {
		diagonal_size = 1.1f * std::sqrt(2.0f) * static_cast<float>(CHUNK_SIZE * WORLD_SIZE);
	}
	else if (quality == HIGH) {
		diagonal_size = 2.0f * std::sqrt(2.0f) * LIGHT_HIGH_DISTANCE;
	}

	return glm::ortho(-diagonal_size / 2.0f, diagonal_size / 2.0f, -diagonal_size / 2.0f, diagonal_size / 2.0f, 0.1f, diagonal_size);
}

glm::mat4 Light::get_transform_matrix(const Camera& camera, ShadowMapQuality quality) const {
	return get_projection_matrix(quality) * get_view_matrix(camera, quality);
}