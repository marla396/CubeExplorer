#pragma once

#include <glm/vec3.hpp>

#include "camera.h"
#include "world/world.h"

class Entity {
public:
	Entity();
	virtual ~Entity();

	virtual void update(const std::shared_ptr<World>& world, Camera& camera, float dt) = 0;

	void set_position(const glm::vec3& pos);

	glm::vec3 get_velocity() const;

protected:
	void update_position(float dt);

	glm::vec3 m_position;
	glm::vec3 m_velocity;
};