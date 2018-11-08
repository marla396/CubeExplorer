#pragma once

#include "entity.h"

class Player : public Entity {
public:
	Player();

	void update(const std::shared_ptr<World>& world, Camera& camera, float dt);
private:
	bool m_on_ground;
};
