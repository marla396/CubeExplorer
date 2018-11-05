#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <algorithm>

#include "camera.h"
#include "application.h"


Camera::Camera() : m_position(0.0f, 0.0f, 0.0f), m_rotation({ 0.0f, 0.0f, 0.0f }) {

}

Camera::~Camera() {

}

void Camera::process_mouse(float dx, float dy) {
	//return;
	m_rotation.x += (dy / static_cast<float>(Application::get_height())) * MAX_PITCH * 2;
	m_rotation.x = std::max(-MAX_PITCH, std::min(MAX_PITCH, m_rotation.x));

	m_rotation.y += (dx / static_cast<float>(Application::get_width())) * PI * 2;

	if (m_rotation.y > PI * 2)
		m_rotation.y -= PI * 2;
	else if (m_rotation.y < 0)
		m_rotation.y += PI * 2;
}

void Camera::process_keyboard(float dt) {

	UNUSED(dt);

	float sin_rot_x = std::sin(m_rotation.x);
	float cos_rot_x = std::cos(m_rotation.x);

	float sin_rot_y = std::sin(m_rotation.y);
	float cos_rot_y = std::cos(m_rotation.y);

	if (Application::key_down(GLFW_KEY_W)) {
		m_position.x += sin_rot_y *cos_rot_x;
		m_position.y -= sin_rot_x;
		m_position.z -= cos_rot_y *cos_rot_x;
	}

	if (Application::key_down(GLFW_KEY_S)) {
		m_position.x -= sin_rot_y *cos_rot_x;
		m_position.y += sin_rot_x;
		m_position.z += cos_rot_y *cos_rot_x;
	}

	if (Application::key_down(GLFW_KEY_A)) {
		m_position.x -= cos_rot_y;
		m_position.z -= sin_rot_y;
	}

	if (Application::key_down(GLFW_KEY_D)) {
		m_position.x += cos_rot_y;
		m_position.z += sin_rot_y;
	}
}

void Camera::set_position(const glm::vec3& position) {
	m_position = position;
}

glm::vec3 Camera::get_position() const {
	return m_position;
}

void Camera::set_rotation(const glm::vec3& rotation) {
	m_rotation = rotation;
}

glm::vec3 Camera::get_rotation() const {
	return m_rotation;
}

void Camera::set_yaw(float yaw) {
	m_rotation.y = yaw;
}
void Camera::set_pitch(float pitch) {
	m_rotation.x = pitch; //?????
}

void Camera::set_near(float znear) {
	m_near = znear;
}

float Camera::get_near() const {
	return m_near;
}

void Camera::set_far(float zfar) {
	m_far = zfar;
}

float Camera::get_far() const {
	return m_far;
}

void Camera::set_aspect(float aspect) {
	m_aspect = aspect;
}

float Camera::get_aspect() const {
	return m_aspect;
}

void Camera::set_fov(float fov) {
	m_fov = fov;
}

float Camera::get_fov() const {
	return m_fov;
}

std::array<glm::vec4, 6> Camera::get_frustum_planes() const {

	std::array<glm::vec4, 6> ret;

	ret[0].x = m_projection_matrix[0][3] + m_projection_matrix[0][0];
	ret[0].y = m_projection_matrix[1][3] + m_projection_matrix[1][0];
	ret[0].z = m_projection_matrix[2][3] + m_projection_matrix[2][0];
	ret[0].w = m_projection_matrix[3][3] + m_projection_matrix[3][0];

	ret[1].x = m_projection_matrix[0][3] - m_projection_matrix[0][0];
	ret[1].y = m_projection_matrix[1][3] - m_projection_matrix[1][0];
	ret[1].z = m_projection_matrix[2][3] - m_projection_matrix[2][0];
	ret[1].w = m_projection_matrix[3][3] - m_projection_matrix[3][0];

	ret[2].x = m_projection_matrix[0][3] - m_projection_matrix[0][1];
	ret[2].y = m_projection_matrix[1][3] - m_projection_matrix[1][1];
	ret[2].z = m_projection_matrix[2][3] - m_projection_matrix[2][1];
	ret[2].w = m_projection_matrix[3][3] - m_projection_matrix[3][1];

	ret[3].x = m_projection_matrix[0][3] + m_projection_matrix[0][1];
	ret[3].y = m_projection_matrix[1][3] + m_projection_matrix[1][1];
	ret[3].z = m_projection_matrix[2][3] + m_projection_matrix[2][1];
	ret[3].w = m_projection_matrix[3][3] + m_projection_matrix[3][1];

	ret[4].x = m_projection_matrix[0][2];
	ret[4].y = m_projection_matrix[1][2];
	ret[4].z = m_projection_matrix[2][2];
	ret[4].w = m_projection_matrix[3][2];

	ret[5].x = m_projection_matrix[0][3] - m_projection_matrix[0][2];
	ret[5].y = m_projection_matrix[1][3] - m_projection_matrix[1][2];
	ret[5].z = m_projection_matrix[2][3] - m_projection_matrix[2][2];
	ret[5].w = m_projection_matrix[3][3] - m_projection_matrix[3][2];

	return ret;
}


glm::mat4 Camera::get_view_matrix() {
	update_view_matrix();
	return m_view_matrix;
}

glm::mat4 Camera::get_projection_matrix() {
	update_projection_matrix();
	return m_projection_matrix;
}

void Camera::update_view_matrix() {
	m_view_matrix = glm::rotate(glm::identity<glm::mat4>(), m_rotation.x, { 1, 0, 0 });
	m_view_matrix = glm::rotate(m_view_matrix, m_rotation.y, { 0, 1, 0 });
	m_view_matrix = glm::rotate(m_view_matrix, m_rotation.z, { 0, 0, 1 });

	m_view_matrix = glm::translate(m_view_matrix, -m_position);
}

void Camera::update_projection_matrix() {
	m_projection_matrix = glm::perspective(m_fov, m_aspect, m_near, m_far);
}
