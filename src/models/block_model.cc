#include "models/block_model.h"
#include "texture/texture_atlas.h"

BlockModel::BlockModel(const std::shared_ptr<TextureAtlas<32, 32>>& texture) {
	set_texture(texture);
	generate_geometry();
	upload_geometry();
}

void BlockModel::generate_geometry() {
	m_vertices = {
		//Bottom
		-0.5f, 0.0f, 0.5f,	//0
		0.5f, 0.0f, 0.5f,	//1
		-0.5f, 0.0f, -0.5f,	//2
		0.5f, 0.0f, -0.5f,	//3


		//Top
		-0.5f, 1.0f, 0.5f,	//4
		0.5f, 1.0f, 0.5f,	//5
		-0.5f, 1.0f, -0.5f,	//6
		0.5f, 1.0f, -0.5f,	//7


		//Front
		-0.5f, 0.0f, 0.5f,
		0.5f, 0.0f, 0.5f,
		-0.5f, 1.0f, 0.5f,
		0.5f, 1.0f, 0.5f,


		//Back
		-0.5f, 0.0f, -0.5f,
		0.5f, 0.0f, -0.5f,
		-0.5f, 1.0f, -0.5f,
		0.5f, 1.0f, -0.5f,


		//Left
		-0.5f, 0.0f, 0.5f,
		-0.5f, 0.0f, -0.5f,
		-0.5f, 1.0f, 0.5f,
		-0.5f, 1.0f, -0.5f,


		//Right
		0.5f, 0.0f, 0.5f,
		0.5f, 0.0f, -0.5f,
		0.5f, 1.0f, 0.5f,
		0.5f, 1.0f, -0.5f,


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

	
	auto top = m_texture->get_texture_coords(0, 0);
	auto sides = m_texture->get_texture_coords(1, 0);
	auto bottom = m_texture->get_texture_coords(2, 0);
	m_tex_coords.insert(m_tex_coords.end(), bottom.begin(), bottom.end());
	m_tex_coords.insert(m_tex_coords.end(), top.begin(), top.end());

	for (int i = 2; i < 6; i++) {
		m_tex_coords.insert(m_tex_coords.end(), sides.begin(), sides.end());
	}
}