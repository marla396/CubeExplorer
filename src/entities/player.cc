#include "entities/player.h"
#include "entities/physics_const.h"
#include "application.h"


Player::Player() : m_on_ground(false) {

}

void Player::update(const std::shared_ptr<World>& world, Camera& camera, float dt) {


	auto chunks = world->get_chunks();
	auto rotation = camera.get_rotation();
	

	if (Application::key_down(GLFW_KEY_W)) {

		m_position.x += std::sin(rotation.y) * PHYS_ENTITY_ACC * dt;
		m_position.z -= std::cos(rotation.y) * PHYS_ENTITY_ACC * dt;
	}

	if (Application::key_down(GLFW_KEY_S)) {

		m_position.x -= std::sin(rotation.y) * PHYS_ENTITY_ACC * dt;
		m_position.z += std::cos(rotation.y) * PHYS_ENTITY_ACC * dt;
	}

	if (Application::key_down(GLFW_KEY_A)) {

		m_position.x -= std::cos(rotation.y) * PHYS_ENTITY_ACC * dt;
		m_position.z -= std::sin(rotation.y) * PHYS_ENTITY_ACC * dt;
	}

	if (Application::key_down(GLFW_KEY_D)) {

		m_position.x += std::cos(rotation.y) * PHYS_ENTITY_ACC * dt;
		m_position.z += std::sin(rotation.y) * PHYS_ENTITY_ACC * dt;
	}

	if (Application::key_down(GLFW_KEY_SPACE) && m_on_ground) {
		m_velocity.y += PHYS_JUMP_VELOCITY;
	}

	/*if (std::abs(m_velocity.x) > 0.1f) {

		if (m_velocity.x > 0) {
			m_velocity.x -= PHYS_FRICTION * dt;
		}
		else {
			m_velocity.x += PHYS_FRICTION * dt;
		}
	}
	else {
		m_velocity.x = 0.0f;
	}

	m_velocity.x = std::max(-PHYS_MAX_VELOCITY_XZ, std::min(PHYS_MAX_VELOCITY_XZ, m_velocity.x));

	if (std::abs(m_velocity.z) > 0.1f) {

		if (m_velocity.z > 0) {
			m_velocity.z -= PHYS_FRICTION * dt;
		}
		else {
			m_velocity.z += PHYS_FRICTION * dt;
		}
	}
	else {
		m_velocity.z = 0.0f;
	}

	m_velocity.z = std::max(-PHYS_MAX_VELOCITY_XZ, std::min(PHYS_MAX_VELOCITY_XZ, m_velocity.z));*/


	m_velocity.y += PHYS_GRAVITY * dt;
	m_velocity.y = std::max(-PHYS_MAX_VELOCITY_Y, std::min(m_velocity.y, PHYS_MAX_VELOCITY_Y));

	m_position += m_velocity * dt;

	float height = world->height_at(m_position.x, m_position.z) + 1.0f;

	if (m_position.y <= height) {
		m_position.y = height;
		m_velocity.y = 0.0f;
		m_on_ground = true;
	}
	else {
		m_on_ground = false;
	}

	camera.set_position(m_position);
}

bool Player::find_chunk_below(const glm::ivec3& pos, glm::ivec3& out, const std::shared_ptr<World>& world) const {
	const int MAX_TRIES_Y = 5;
	out = pos;

	auto chunks = world->get_chunks();

	for (int i = 0; i < MAX_TRIES_Y; i++) {
		out.y -= i;
		auto it = chunks->find(out);

		if (it != chunks->end()) {
			return true;
		}
	}
	return false;
}