#include "world/light.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/matrix.hpp>
#include <limits>

#include "world/world_const.h"


Light::Light(const glm::vec3& position, const glm::vec3& color) 
	: m_position(position), m_color(color){

}

void Light::update(Camera& camera, float time) {
	float diagonal_size;

	diagonal_size = 1.1f * std::sqrt(2.0f) * static_cast<float>(CHUNK_SIZE * WORLD_SIZE);
	m_projection_matrices[0] = glm::ortho(-diagonal_size / 2.0f, diagonal_size / 2.0f, -diagonal_size / 2.0f, diagonal_size / 2.0f, 0.1f, diagonal_size);
	m_projection_matrices[1] = m_projection_matrices[0];
	m_view_matrices[0] = glm::lookAt(m_position, WORLD_CENTER, { 0.0f, 1.0f, 0.0f });
	m_view_matrices[1] = m_view_matrices[0];

	for (int i = 0; i < SHADOW_CASCADES; i++) {
		m_transform_matrices[i] = m_projection_matrices[i] * m_view_matrices[i];
	}

	float frustum_length = camera.get_far();	

	/*for (int i = 0; i < SHADOW_CASCADES; i++) {
		m_shadow_cascades[i] = camera.get_near() + static_cast<float>(i) * (frustum_length / static_cast<float>(SHADOW_CASCADES));
	}*/

	const float split[] = {
			-0.015f, 0.015f,
			-0.025f, 0.05f,
			0.0f, 0.07f,
			0.06f, 0.14f,
			0.12f, 0.34f,
			0.3f, 0.6f
	};

	for (int i = 0; i < SHADOW_CASCADES * 2; i += 2) {
		m_shadow_cascades[i / 2] = { frustum_length * split[i], frustum_length * split[i + 1] };
	}


	//m_shadow_cascades[SHADOW_CASCADES] = camera.get_far();

	glm::mat4 view = camera.get_view_matrix();
	glm::mat4 view_inv = glm::inverse(camera.get_view_matrix());
	//glm::mat4 vm = glm::lookAt({ 0.0f, 0.0f, 0.0f }, WORLD_CENTER, { 0.0f, 1.0f, 0.0f });

	glm::vec3 f(glm::normalize(WORLD_CENTER - m_position));
	glm::vec3 s(glm::normalize(glm::cross(f, { 0.0f, 1.0f, 0.0f })));
	glm::vec3 u(glm::cross(s, f));

	glm::mat4 vm = glm::identity<glm::mat4>();
	vm[0][0] = s.x;
	vm[1][0] = s.y;
	vm[2][0] = s.z;
	vm[0][1] = u.x;
	vm[1][1] = u.y;
	vm[2][1] = u.z;
	vm[0][2] = -f.x;
	vm[1][2] = -f.y;
	vm[2][2] = -f.z;
	vm[3][3] = 1.0f;

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
			v = glm::vec3(vm * t);
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
		m_view_matrices[i] = vm;
		m_transform_matrices[i] = m_projection_matrices[i] * vm;
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

glm::mat4 Light::get_view_matrix(int cascade) const {
	return m_view_matrices[cascade];
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