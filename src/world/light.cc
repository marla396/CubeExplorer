#include "world/light.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/matrix.hpp>
#include <limits>

#include "world/world_const.h"


Light::Light(const glm::vec3& position, const glm::vec3& color) 
	: m_position(position), m_color(color){

}

void Light::update(Camera& camera, float) {

	float frustum_length = camera.get_far() - camera.get_near();	

	const float split[] = {
			-0.005f, 0.005f,
			-0.015f, 0.015f,
			-0.025f, 0.04f,
			0.03f, 0.1f,
			0.09f, 0.25f,
			0.24f, 0.6f
	};

	for (int i = 0; i < SHADOW_CASCADES * 2; i += 2) {
		m_shadow_cascades[i / 2] = { frustum_length * split[i], frustum_length * split[i + 1] };
	}


	glm::vec3 f(glm::normalize(WORLD_CENTER - m_position));
	glm::vec3 s(glm::normalize(glm::cross(f, { 0.0f, 1.0f, 0.0f })));
	glm::vec3 u(glm::cross(s, f));

	m_view_matrix = glm::identity<glm::mat4>();
	m_view_matrix[0][0] = s.x;
	m_view_matrix[1][0] = s.y;
	m_view_matrix[2][0] = s.z;
	m_view_matrix[0][1] = u.x;
	m_view_matrix[1][1] = u.y;
	m_view_matrix[2][1] = u.z;
	m_view_matrix[0][2] = -f.x;
	m_view_matrix[1][2] = -f.y;
	m_view_matrix[2][2] = -f.z;
	m_view_matrix[3][3] = 1.0f;

	glm::vec3 up = camera.get_up();
	glm::vec3 forward = camera.get_forward();
	glm::vec3 right = camera.get_right();

	float tan_fov = std::tan(camera.get_fov() / 2.0f);

	for (int i = 0; i < SHADOW_CASCADES; i++) {
		float height_near = tan_fov * m_shadow_cascades[i].x;
		float width_near = height_near * camera.get_aspect();

		float height_far = tan_fov * m_shadow_cascades[i].y;
		float width_far = height_far * camera.get_aspect();

		glm::vec3 center_near = camera.get_position() + m_shadow_cascades[i].x * forward;
		glm::vec3 center_far = camera.get_position() + m_shadow_cascades[i].y * forward;

		std::array<glm::vec3, 8> corners = {
			//Near corners
			center_near + up * height_near - (width_near * right),
			center_near + up * height_near + (width_near * right),
			center_near - up * height_near - (width_near * right),
			center_near - up * height_near + (width_near * right),

			//Far corners
			center_far + up * height_far - (width_far * right),
			center_far + up * height_far + (width_far * right),
			center_far - up * height_far - (width_far * right),
			center_far - up * height_far + (width_far * right),

		};

		//Transform to light space coordinates
		for (auto &v : corners) {
			glm::vec4 t = glm::vec4(v, 1.0f);
			v = glm::vec3(m_view_matrix * t);
		}

		float min_x, min_y, min_z;
		float max_x, max_y, max_z;

		min_x = min_y = min_z = 1000000.0f;
		max_x = max_y = max_z = -1000000.0f;

		std::array<glm::vec4, 8> corners_l;

		for (int j = 0; j < 8; j++) {

			glm::vec3 corner = corners[j];

			min_x = std::min(min_x, corner.x);
			min_y = std::min(min_y, corner.y);
			min_z = std::min(min_z, corner.z);

			max_x = std::max(max_x, corner.x);
			max_y = std::max(max_y, corner.y);
			max_z = std::max(max_z, corner.z);
		}
		
		m_projection_matrices[i] = glm::ortho(min_x - 10.0f, max_x + 10.0f, min_y - 10.0f, max_y + 10.0f, -max_z - 10.0f, -min_z + 10.0f);
		m_transform_matrices[i] = m_projection_matrices[i] * m_view_matrix;
	}
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

glm::mat4 Light::get_view_matrix() const {
	return m_view_matrix;
}

glm::mat4 Light::get_projection_matrix(int cascade) const {
	return m_projection_matrices[cascade];
}

glm::mat4 Light::get_transform_matrix(int cascade) const {
	return m_transform_matrices[cascade];
}

float Light::get_shadow_cascade_end(int cascade) const {
	return m_shadow_cascades[cascade].y;
}