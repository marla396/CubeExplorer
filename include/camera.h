#pragma once

#include <array>
#include <glm/glm.hpp>
#include <memory>


#include "gl.h"
#include "lazy_object.h"

struct FrustumPlane {
	glm::vec3 n;
	float d;
};

constexpr int FRUSTUM_RIGHT = 0;
constexpr int FRUSTUM_LEFT = 1;
constexpr int FRUSTUM_BOTTOM = 2;
constexpr int FRUSTUM_TOP = 3;
constexpr int FRUSTUM_NEAR = 4;
constexpr int FRUSTUM_FAR = 5;

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

	void toggle_lock_frustum();

	std::array<FrustumPlane, 6> get_frustum_planes() const;

	glm::mat4 get_view_matrix();
	glm::mat4 get_projection_matrix();
private:
	glm::mat4 update_view_matrix();
	glm::mat4 update_projection_matrix();
	std::array<FrustumPlane, 6> update_frustum_planes();

	void notify_projection() const;
	void notify_view() const;

	glm::vec3 m_position;
	glm::vec3 m_rotation;

	float m_near;
	float m_far;
	float m_fov;
	float m_aspect;

	bool m_lock_frustum;
	glm::mat4 m_locked_view_matrix;

	std::unique_ptr<LazyObject<glm::mat4>> m_view_matrix;
	std::unique_ptr<LazyObject<glm::mat4>> m_projection_matrix;
	std::unique_ptr<LazyObject<std::array<FrustumPlane, 6>>> m_frustum_planes;
};