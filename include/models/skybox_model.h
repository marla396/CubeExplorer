#pragma once

#include "model.h"
#include "texture/ftexture.h"

class SkyboxModel : public Model {
public:
	SkyboxModel(const std::shared_ptr<FTexture>& texture);
private:
	void generate_geometry();
};