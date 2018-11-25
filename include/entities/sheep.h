#pragma once

#include "entity.h"
#include "models/sheep_model.h"
#include "misc/astar.h"


class Sheep : public Entity {
public:
	Sheep(const std::shared_ptr<ITexture>& texture);

	void update(const std::shared_ptr<World>& world, Camera& camera, float dt);

private:
	void evaluate_plan(const std::shared_ptr<World>& world);
	glm::vec3 get_walking_direction(const AStar::NodePtr& node) const;

	std::vector<AStar::NodePtr> m_current_plan;
	bool m_has_plan;
	bool m_on_ground;
};
