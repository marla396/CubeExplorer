#include "models/skybox_model.h"

SkyboxModel::SkyboxModel(const std::shared_ptr<FTexture>& texture) {
	set_texture(texture);
	generate_geometry();
	upload_geometry();
}

void SkyboxModel::generate_geometry() {
	m_vertices = {
		//Bottom
		-2.0f, -2.0f, 2.0f,	//0
		2.0f, -2.0f, 2.0f,	//1
		-2.0f, -2.0f, -2.0f,	//2
		2.0f, -2.0f, -2.0f,	//3

		//Top
		-2.0f, 2.0f, 2.0f,	//4
		2.0f, 2.0f, 2.0f,	//5
		-2.0f, 2.0f, -2.0f,	//6
		2.0f, 2.0f, -2.0f,	//7

		//Front
		-2.0f, -2.0f, 2.0f,
		2.0f, -2.0f, 2.0f,
		-2.0f, 2.0f, 2.0f,
		2.0f, 2.0f, 2.0f,

		//Back
		-2.0f, -2.0f, -2.0f,
		2.0f, -2.0f, -2.0f,
		-2.0f, 2.0f, -2.0f,
		2.0f, 2.0f, -2.0f,

		//Left
		-2.0f, -2.0f, 2.0f,
		-2.0f, -2.0f, -2.0f,
		-2.0f, 2.0f, 2.0f,
		-2.0f, 2.0f, -2.0f,

		//Right
		2.0f, -2.0f, 2.0f,
		2.0f, -2.0f, -2.0f,
		2.0f, 2.0f, 2.0f,
		2.0f, 2.0f, -2.0f,
	};

	m_tex_coords = {
		0.0f, 0.5f,
		0.25f, 0.5f,
		0.0f, 0.0f,
		0.25f, 0.0f,

		0.25f, 0.5f,
		0.5f, 0.5f,
		0.25f, 1.0f,
		0.5f, 1.0f,

		0.25f, 0.0f,
		0.5f, 0.0f,
		0.25f, 0.5f,
		0.5f, 0.5f,

		0.75f, 0.5f,
		0.5f, 0.5f,
		0.75f, 1.0f,
		0.5f, 1.0f,

		0.25f, 0.5f,
		0.0f, 0.5f,
		0.25f, 1.0f,
		0.0f, 1.0f,

		0.5f, 0.0f,
		0.75f, 0.0f,
		0.5f, 0.5f,
		0.75f, 0.5f,
	};

	m_indices = {
		//Bottom
		0, 1, 2,
		1, 3, 2,

		//Top
		4, 6, 5,
		5, 6, 7,

		//Front
		8, 10, 9,
		9, 10, 11,

		//Back
		12, 13, 14,
		13, 15, 14,

		//Left
		16, 17, 18,
		17, 19, 18,

		//Right
		20, 22, 21,
		21, 22, 23,
	};
}