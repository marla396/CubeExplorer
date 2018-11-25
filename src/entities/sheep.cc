#include "entities/sheep.h"
#include "entities/physics_const.h"
#include "world/world.h"

Sheep::Sheep(const std::shared_ptr<ITexture>& texture) : m_has_plan(false) {
	m_model = std::make_shared<SheepModel>(texture);

	auto m = std::dynamic_pointer_cast<SheepModel>(m_model);
	m->gpu_init();
}

void Sheep::update(const std::shared_ptr<World>& world, Camera& camera, float dt) {

	evaluate_plan(world);

	//if (m_on_ground)
	//	m_velocity.y += PHYS_JUMP_VELOCITY;

	m_velocity.y += PHYS_GRAVITY * dt;
	m_velocity.y = std::max(-PHYS_MAX_VELOCITY_Y, std::min(m_velocity.y, PHYS_MAX_VELOCITY_Y));

	m_position += m_velocity * dt;

	float height = world->height_at(m_position.x, m_position.z);

	if (m_position.y <= height) {
		m_position.y = height;
		m_velocity.y = 0.0f;
		m_on_ground = true;
	}
	else {
		m_on_ground = false;
	}

	set_position(m_position);
}

void Sheep::evaluate_plan(const std::shared_ptr<World>& world) {
	if (m_current_plan.empty() || !m_has_plan) {
		m_current_plan.clear();

		glm::vec3 center = WORLD_CENTER;

		center.y = world->height_at(center.x, center.z);

		if (glm::length(m_position - center) > 1.0f) {



			m_has_plan = AStar::path(m_position, center, world->get_height_map(), m_current_plan);
		}
		else {
			m_has_plan = false;
		}
	}

	if (m_has_plan) {
		auto node = m_current_plan.back();
		auto tmp = node;

		while (!m_current_plan.empty() && glm::length(m_position - node->pos) < 1.0f) {
			node = m_current_plan.back();
			m_current_plan.pop_back();
		}

		if (m_position != node->pos) {

			m_position += get_walking_direction(node) * 0.1f;
		}
	}
}

glm::vec3 Sheep::get_walking_direction(const AStar::NodePtr& node) const {

	glm::vec3 delta = node->pos - m_position;
	delta.y = 0.0f;
	return glm::normalize(delta);
}
