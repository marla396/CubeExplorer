#include "entities/entity.h"

Entity::Entity() {
	m_velocity = { 0.0f, 0.0f, 0.0f };
}

Entity::~Entity() {

}

void Entity::update_position(float dt) {
	UNUSED(dt);
}

void Entity::set_position(const glm::vec3& pos) {
	m_position = pos;
}

glm::vec3 Entity::get_velocity() const {
	return m_velocity;
}
