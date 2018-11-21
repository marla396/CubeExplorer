#include "world/light.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/matrix.hpp>
#include <limits>

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

glm::mat4 Light::get_projection_matrix(const Camera& camera, ShadowMapQuality quality) const {

	float diagonal_size;
	
	//if (quality == LOW) {
		diagonal_size = 1.1f * std::sqrt(2.0f) * static_cast<float>(CHUNK_SIZE * WORLD_SIZE);
		return glm::ortho(-diagonal_size / 2.0f, diagonal_size / 2.0f, -diagonal_size / 2.0f, diagonal_size / 2.0f, 0.1f, quality == LOW ? diagonal_size : diagonal_size / 2.0f);
//	}
//	else if (quality == HIGH) {
//		diagonal_size = 1.1f * LIGHT_HIGH_DISTANCE;
//		return glm::ortho(-diagonal_size / 2.0f, diagonal_size / 2.0f, -diagonal_size / 2.0f, diagonal_size / 2.0f, 0.1f, diagonal_size * 2.0f);
//	}

	/*glm::mat4 view = get_view_matrix(camera, quality);

	glm::mat4 inv_view = glm::inverse(view);

	//glm::mat4 lm = glm::lookAt({ 0.0f, 0.0f, 0.0f }, glm::normalize(WORLD_CENTER - m_position), { 0.0f, 1.0f, 0.0 });

	float thhf = std::tan(camera.get_fov() / 2.0);
	float thvf = std::tan((camera.get_fov() * camera.get_aspect()) / 2.0f);

	float cascade_begin;
	float cascade_end;

	if (quality == Light::HIGH) {
		cascade_begin = camera.get_near();
		cascade_end = camera.get_near() + 60.0f;
	}
	else {
		cascade_begin = camera.get_near() + 60.0f;
		cascade_end = camera.get_far();
	}

	float xn = cascade_begin * thhf;
	float xf = cascade_end * thhf;
	float yn = cascade_begin * thvf;
	float yf = cascade_end * thvf;

	std::array<glm::vec4, 8> corners = {
		glm::vec4 { xn, yn, cascade_begin, 1.0 },
		glm::vec4 { -xn, yn, cascade_begin, 1.0 },
		glm::vec4 { xn, -yn, cascade_begin, 1.0 },
		glm::vec4 { -xn, -yn, cascade_begin, 1.0 },
		glm::vec4 { xf, yf, cascade_end, 1.0 },
		glm::vec4 { -xf, yf, cascade_end, 1.0 },
		glm::vec4 { xf, -yf, cascade_end, 1.0 },
		glm::vec4 { -xf, -yf, cascade_end, 1.0 },
	};

	float min_x = std::numeric_limits<float>::max();
	float min_y = std::numeric_limits<float>::max();
	float min_z = std::numeric_limits<float>::max();
	float max_x = std::numeric_limits<float>::min();
	float max_y = std::numeric_limits<float>::min();
	float max_z = std::numeric_limits<float>::min();

	for (auto& c : corners) {
		glm::vec4 vw = inv_view * c;

		c = view * vw;

		min_x = std::min(min_x, c.x);
		min_y = std::min(min_y, c.y);
		min_z = std::min(min_z, c.z);
		max_x = std::max(max_x, c.x);
		max_y = std::max(max_y, c.y);
		max_z = std::max(max_z, c.z);

	}

	return glm::ortho(min_x, max_x, min_y, max_y, min_z, max_z);*/
}

glm::mat4 Light::get_transform_matrix(const Camera& camera, ShadowMapQuality quality) const {
	return get_projection_matrix(camera, quality) * get_view_matrix(camera, quality);
}