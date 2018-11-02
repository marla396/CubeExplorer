#include "models/quad2d_model.h"

Quad2DModel::Quad2DModel(float offset){
	m_vertices_dimensions = 2;

	generate_geometry(offset);
	upload_geometry();
	clear_geometry();
}

void Quad2DModel::generate_geometry(float offset) {

	m_vertices = {
		-1.0f * offset, -1.0f * offset,
		-1.0f * offset, 1.0f,
		1.0f, -1.0f * offset,
		1.0f, 1.0f,
	};

	m_indices = {
		0, 1, 2,
		1, 2, 3
	};

	m_tex_coords = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};
}