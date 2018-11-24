#include "models/player_model.h"

PlayerModel::PlayerModel(const std::shared_ptr<ITexture>& texture) : ChunkModel(texture, { 0.0f, 0.0f, 0.0f }) {
	set_scale({ 1.0f, 1.0f, 1.0f });
	generate_geometry();
}

void PlayerModel::generate_geometry() {
	generate_block(0, 2, 0, { 0.8f, 0.8f, 0.8f }, { 0.0f, 0.2f, 0.0f },
		{
			ChunkModel::PLAYER_HEAD_SIDE,
			ChunkModel::PLAYER_HEAD_SIDE,
			ChunkModel::PLAYER_HEAD_TOP,
			ChunkModel::PLAYER_HEAD_FRONT,
			ChunkModel::PLAYER_HEAD_BACK,
			ChunkModel::PLAYER_HEAD_BOTTOM,
		}
	);

	generate_block(0, 1, 0, { 1.3f, 1.2f, 0.7f }, { 0.0f, 0.0f, 0.0f },
		{
			ChunkModel::PLAYER_TORSO,
			ChunkModel::PLAYER_TORSO,
			ChunkModel::PLAYER_TORSO_TOP,
			ChunkModel::PLAYER_TORSO_FRONT,
			ChunkModel::PLAYER_TORSO,
			ChunkModel::PLAYER_TORSO,
		}
	);

	generate_block(0, 0, 0, { 0.4f, 2.0f, 0.4f }, { -0.3f, -1.0f, 0.0f },
		{
			ChunkModel::PLAYER_LEGS,
			ChunkModel::PLAYER_LEGS,
			ChunkModel::PLAYER_LEGS_BOTTOM,
			ChunkModel::PLAYER_LEGS,
			ChunkModel::PLAYER_LEGS,
			ChunkModel::PLAYER_LEGS_BOTTOM,
		}
	);

	generate_block(0, 0, 0, { 0.4f, 2.0f, 0.4f }, { 0.3f, -1.0f, 0.0f },
		{
			ChunkModel::PLAYER_LEGS,
			ChunkModel::PLAYER_LEGS,
			ChunkModel::PLAYER_LEGS_BOTTOM,
			ChunkModel::PLAYER_LEGS,
			ChunkModel::PLAYER_LEGS,
			ChunkModel::PLAYER_LEGS_BOTTOM,
		}
	);

	generate_block(1, 1, 0, { 0.3f, 1.0f, 0.3f }, { -0.25f, 0.2f, 0.0f },
		{
			ChunkModel::PLAYER_ARM,
			ChunkModel::PLAYER_ARM,
			ChunkModel::PLAYER_TORSO_TOP,
			ChunkModel::PLAYER_ARM,
			ChunkModel::PLAYER_ARM,
			ChunkModel::PLAYER_HEAD_BOTTOM,
		}
	);

	generate_block(-1, 1, 0, { 0.3f, 1.0f, 0.3f }, { 0.25f, 0.2f, 0.0f },
		{
			ChunkModel::PLAYER_ARM,
			ChunkModel::PLAYER_ARM,
			ChunkModel::PLAYER_TORSO_TOP,
			ChunkModel::PLAYER_ARM,
			ChunkModel::PLAYER_ARM,
			ChunkModel::PLAYER_HEAD_BOTTOM,
		}
		);
}

void PlayerModel::generate_block(int x, int y, int z, const glm::vec3& scale, const glm::vec3& offset, const std::array<ChunkModel::BlockTexture, 6>& tex) {
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
