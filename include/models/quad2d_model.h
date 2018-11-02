#pragma once

#include "model.h"

class Quad2DModel : public Model {
public:
	Quad2DModel(float offset = 0.0f);

	void generate_geometry(float offset);
private:

};