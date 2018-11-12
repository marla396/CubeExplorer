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

	if (m_model) {
		m_model->set_position(pos);
	}
}

glm::vec3 Entity::get_velocity() const {
	return m_velocity;
}

std::shared_ptr<Model> Entity::get_model() const {
	return m_model;
}
