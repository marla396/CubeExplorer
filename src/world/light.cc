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

	/*float height = std::tan(camera.get_fov() / 2.0f) * (camera.get_far() + camera.get_near()) / 4.0f;
	float left = -height * camera.get_aspect();
	float right = height * camera.get_aspect();
	float bottom = -height;
	float top = height;

	return glm::ortho(left, right, bottom, top, camera.get_near(), camera.get_far());*/

	return glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 400.0f);

	//return glm::perspective(PI / 4.0f, 1.0f, 0.01f, 1024.0f);
}

glm::mat4 Light::get_transform_matrix(const Camera& camera) const {
	return get_projection_matrix(camera) * get_view_matrix(camera);
}