#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <algorithm>

#include "camera.h"
#include "application.h"


Camera::Camera() 
	: m_position(0.0f, 0.0f, 0.0f), m_rotation({ 0.0f, 0.0f, 0.0f }), m_lock_frustum(false) {

	m_view_matrix = std::make_unique<LazyObject<glm::mat4>>([this]() {
		return update_view_matrix();
	});

	m_projection_matrix = std::make_unique<LazyObject<glm::mat4>>([this]() {
		return update_projection_matrix();
	});

	m_frustum_planes = std::make_unique<LazyObject<std::array<FrustumPlane, 6>>>([this]() {
		return update_frustum_planes();
	});

}

Camera::~Camera() {

}

void Camera::process_mouse(float dx, float dy) {
	m_rotation.x += (dy / static_cast<float>(Application::get_height())) * MAX_PITCH * 2;
	m_rotation.x = std::max(-MAX_PITCH, std::min(MAX_PITCH, m_rotation.x));

	m_rotation.y += (dx / static_cast<float>(Application::get_width())) * PI * 2;

	if (m_rotation.y > PI * 2)
		m_rotation.y -= PI * 2;
	else if (m_rotation.y < 0)
		m_rotation.y += PI * 2;

	notify_view();
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

	notify_view();
}

void Camera::set_position(const glm::vec3& position) {
	m_position = position;
	notify_view();
}

glm::vec3 Camera::get_position() const {
	return m_position;
}

void Camera::set_rotation(const glm::vec3& rotation) {
	m_rotation = rotation;
	notify_view();
}

glm::vec3 Camera::get_rotation() const {
	return m_rotation;
}

float Camera::get_yaw() const {
	return m_rotation.y;
}

void Camera::set_yaw(float yaw) {
	m_rotation.y = yaw;
	notify_view();
}

float Camera::get_pitch() const {
	return m_rotation.x;
}

void Camera::set_pitch(float pitch) {
	m_rotation.x = pitch;
	notify_view();
}

void Camera::set_near(float znear) {
	m_near = znear;
	notify_projection();
}

float Camera::get_near() const {
	return m_near;
}

void Camera::set_far(float zfar) {
	m_far = zfar;
	notify_projection();
}

float Camera::get_far() const {
	return m_far;
}

void Camera::set_aspect(float aspect) {
	m_aspect = aspect;
	notify_projection();
}

float Camera::get_aspect() const {
	return m_aspect;
}

void Camera::set_fov(float fov) {
	m_fov = fov;
	notify_projection();
}

float Camera::get_fov() const {
	return m_fov;
}

void Camera::toggle_lock_frustum() {
	m_lock_frustum = !m_lock_frustum;

	if (m_lock_frustum) {
		m_locked_view_matrix = m_view_matrix->get();
	}
}

std::array<FrustumPlane, 6> Camera::get_frustum_planes() const {
	return m_frustum_planes->get();
}

glm::mat4 Camera::get_view_matrix() {
	return m_view_matrix->get();
}

glm::mat4 Camera::get_projection_matrix() {	
	return m_projection_matrix->get();
}

glm::mat4 Camera::update_view_matrix() {

	glm::mat4 view_matrix = glm::identity<glm::mat4>();

	view_matrix = glm::rotate(view_matrix, m_rotation.x, { 1.0f, 0.0f, 0.0f });
	view_matrix = glm::rotate(view_matrix, m_rotation.y, { 0.0f, 1.0f, 0.0f });
	view_matrix = glm::rotate(view_matrix, m_rotation.z, { 0.0f, 0.0f, 1.0f });

	return glm::translate(view_matrix, -m_position);
}

glm::mat4 Camera::update_projection_matrix() {
	return glm::perspective(m_fov, m_aspect, m_near, m_far);
}

std::array<FrustumPlane, 6> Camera::update_frustum_planes() {

	//https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf

	std::array<FrustumPlane, 6> p;

	glm::mat4 view = m_lock_frustum ? m_locked_view_matrix : m_view_matrix->get();

	glm::mat4 m = glm::transpose(m_projection_matrix->get() * view);

	p[FRUSTUM_LEFT].n.x = m[3][0] + m[0][0];
	p[FRUSTUM_LEFT].n.y = m[3][1] + m[0][1];
	p[FRUSTUM_LEFT].n.z = m[3][2] + m[0][2];
	p[FRUSTUM_LEFT].d = m[3][3] + m[0][3];

	p[FRUSTUM_RIGHT].n.x = m[3][0] - m[0][0];
	p[FRUSTUM_RIGHT].n.y = m[3][1] - m[0][1];
	p[FRUSTUM_RIGHT].n.z = m[3][2] - m[0][2];
	p[FRUSTUM_RIGHT].d = m[3][3] - m[0][3];

	p[FRUSTUM_BOTTOM].n.x = m[3][0] + m[1][0];
	p[FRUSTUM_BOTTOM].n.y = m[3][1] + m[1][1];
	p[FRUSTUM_BOTTOM].n.z = m[3][2] + m[1][2];
	p[FRUSTUM_BOTTOM].d = m[3][3] + m[1][3];

	p[FRUSTUM_TOP].n.x = m[3][0] - m[1][0];
	p[FRUSTUM_TOP].n.y = m[3][1] - m[1][1];
	p[FRUSTUM_TOP].n.z = m[3][2] - m[1][2];
	p[FRUSTUM_TOP].d = m[3][3] - m[1][3];

	p[FRUSTUM_NEAR].n.x = m[3][0] + m[2][0];
	p[FRUSTUM_NEAR].n.y = m[3][1] + m[2][1];
	p[FRUSTUM_NEAR].n.z = m[3][2] + m[2][2];
	p[FRUSTUM_NEAR].d = m[3][3] + m[2][3];

	p[FRUSTUM_FAR].n.x = m[3][0] - m[2][0];
	p[FRUSTUM_FAR].n.y = m[3][1] - m[2][1];
	p[FRUSTUM_FAR].n.z = m[3][2] - m[2][2];
	p[FRUSTUM_FAR].d = m[3][3] - m[2][3];



	for (int i = 0; i < 6; i++) {
		float mag = std::sqrt(p[i].n.x * p[i].n.x + p[i].n.y * p[i].n.y + p[i].n.z * p[i].n.z);
		p[i].n /= mag;
		p[i].d /= mag;
	}

	return p;
}

void Camera::notify_projection() const {
	m_projection_matrix->notify();
	m_frustum_planes->notify();
}

void Camera::notify_view() const {
	m_view_matrix->notify();
	m_frustum_planes->notify();
}