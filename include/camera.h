#pragma once

#include <array>
#include <glm/glm.hpp>

#include "gl.h"

class Camera {
public:
	Camera();
	~Camera();

	const float MAX_PITCH = PI / 2;

	void process_mouse(float dx, float dy);
	void process_keyboard(float dt);

	void set_position(const glm::vec3& position);
	glm::vec3 get_position() const;

	void set_rotation(const glm::vec3& rotation);
	glm::vec3 get_rotation() const;

	void set_yaw(float yaw);
	void set_pitch(float pitch);

	void set_near(float near);
	float get_near() const;

	void set_far(float far);
	float get_far() const;

	void set_aspect(float aspect);
	float get_aspect() const;

	void set_fov(float fov);
	float get_fov() const;

	std::array<glm::vec4, 6> get_frustum_planes() const;

	glm::mat4 get_view_matrix();
	glm::mat4 get_projection_matrix();
private:
	void update_view_matrix();
	void update_projection_matrix();

	glm::vec3 m_position;
	glm::vec3 m_rotation;

	float m_near;
	float m_far;
	float m_fov;
	float m_aspect;

	glm::mat4 m_view_matrix;
	glm::mat4 m_projection_matrix;
};