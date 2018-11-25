#include "models/sheep_model.h"

SheepModel::SheepModel(const std::shared_ptr<ITexture>& texture) : ChunkModel(texture, { 0.0f, 0.0f, 0.0f }) {
	set_scale({ 1.0f, 1.0f, 1.0f });
	generate_geometry();
}

void SheepModel::generate_geometry() {
	//Head
	generate_block(0, 1, 1, { 0.7f, 0.5f, 0.7f }, { 0.0f, 0.05f, -0.15f },
		{
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD_FRONT,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
		}
	);

	//Body
	generate_block(0, 1, 0, { 1.0f, 1.0f, 1.5f }, { 0.0f, -0.2f, 0.0f },
		{
			ChunkModel::SHEEP_BODY,
			ChunkModel::SHEEP_BODY,
			ChunkModel::SHEEP_BODY,
			ChunkModel::SHEEP_BODY,
			ChunkModel::SHEEP_BODY,
			ChunkModel::SHEEP_BODY,
		}
	);

	//Leg
	generate_block(0, 0, 0, { 0.2f, 0.8f, 0.2f }, { 0.3f, 0.2f, 0.5f },
		{
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
		}
	);

	//Leg
	generate_block(0, 0, 0, { 0.2f, 0.8f, 0.2f }, { -0.3f, 0.2f, 0.5f },
		{
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
		}
	);

	//Leg
	generate_block(0, 0, 0, { 0.2f, 0.8f, 0.2f }, { -0.3f, 0.2f, -0.5f },
		{
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
		}
	);

	generate_block(0, 0, 0, { 0.2f, 0.8f, 0.2f }, { 0.3f, 0.2f, -0.5f },
		{
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
			ChunkModel::SHEEP_HEAD,
		}
	);

}

void SheepModel::generate_block(int x, int y, int z, const glm::vec3& scale, const glm::vec3& offset, const std::array<ChunkModel::BlockTexture, 6>& tex) {
	generate_face(x, y, z, POSITIVE, right_face, scale, offset);
	generate_face_texture(tex[0]);
	generate_normals(right_normal);

	generate_face(x, y, z, NEGATIVE, left_face, scale, offset);
	generate_face_texture(tex[1]);
	generate_normals(left_normal);

	generate_face(x, y, z, POSITIVE, top_face, scale, offset);
	generate_face_texture(tex[2]);
	generate_normals(top_normal);

	generate_face(x, y, z, POSITIVE, front_face, scale, offset);
	generate_face_texture(tex[3]);
	generate_normals(front_normal);

	generate_face(x, y, z, NEGATIVE, back_face, scale, offset);
	generate_face_texture(tex[4]);
	generate_normals(back_normal);

	generate_face(x, y, z, NEGATIVE, bottom_face, scale, offset);
	generate_face_texture(tex[5]);
	generate_normals(bottom_normal);
}