#pragma once

#include "entity.h"
#include "models/player_model.h"

class Player : public Entity {
public:
	Player(const std::shared_ptr<ITexture>& texture);

	void update(const std::shared_ptr<World>& world, Camera& camera, float dt);
private:
	bool m_on_ground;
};
