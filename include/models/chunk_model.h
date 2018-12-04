#pragma once

#include <unordered_map>
#include "model.h"
#include "world/block_map.h"


const std::array<float, 12> bottom_face = {
	-0.5f, 0.0f, 0.5f,	//0
	0.5f, 0.0f, 0.5f,	//1
	-0.5f, 0.0f, -0.5f,	//2
	0.5f, 0.0f, -0.5f,	//3
};

const std::array<float, 3> bottom_normal = {
	0.0f, -1.0f, 0.0f,
};

const std::array<float, 12> top_face = {
	-0.5f, 1.0f, 0.5f,	//4
	0.5f, 1.0f, 0.5f,	//5
	-0.5f, 1.0f, -0.5f,	//6
	0.5f, 1.0f, -0.5f,	//7
};

const std::array<float, 3> top_normal = {
	0.0f, 1.0f, 0.0f,
};

const std::array<float, 12> front_face = {
	-0.5f, 0.0f, 0.5f,
	0.5f, 0.0f, 0.5f,
	-0.5f, 1.0f, 0.5f,
	0.5f, 1.0f, 0.5f,
};

const std::array<float, 3> front_normal = {
	0.0f, 0.0f, 1.0f,
};

const std::array<float, 12> back_face = {
	-0.5f, 0.0f, -0.5f,
	0.5f, 0.0f, -0.5f,
	-0.5f, 1.0f, -0.5f,
	0.5f, 1.0f, -0.5f,
};

const std::array<float, 3> back_normal = {
	0.0f, 0.0f, -1.0f,
};

const std::array<float, 12> left_face = {
	-0.5f, 0.0f, 0.5f,
	-0.5f, 0.0f, -0.5f,
	-0.5f, 1.0f, 0.5f,
	-0.5f, 1.0f, -0.5f,
};

const std::array<float, 3> left_normal = {
	-1.0f, 0.0f, 0.0f,
};

const std::array<float, 12> right_face = {
	0.5f, 0.0f, 0.5f,
	0.5f, 0.0f, -0.5f,
	0.5f, 1.0f, 0.5f,
	0.5f, 1.0f, -0.5f,
};

const std::array<float, 3> right_normal = {
	1.0f, 0.0f, 0.0f,
};

class ChunkModel : public Model {
public:
	ChunkModel(const std::shared_ptr<ITexture>& texture, const glm::vec3& position, bool late_init = false);
	ChunkModel(const std::shared_ptr<ITexture>& texture, const glm::vec3& position, const BlockMap& block_map);
	~ChunkModel();

	void gpu_init();
	bool is_initialized() const;
	void generate_geometry(const BlockMap& block_map);

	glm::vec3 get_center() const override;
	float get_bounding_radius() const override;

protected:
	enum FaceDirection {
		POSITIVE,
		NEGATIVE,
		POSITIVE_NEGATIVE
	};

	enum BlockTexture {
		GRASS,
		GRASS_SOIL_BORDER,
		SOIL,
		SAND,
		TREE_TRUNK,
		TREE_TRUNK_INSIDE,
		TREE_BRANCH,
		PLAYER_HEAD_FRONT,
		PLAYER_HEAD_BACK,
		PLAYER_HEAD_TOP,
		PLAYER_HEAD_SIDE,
		PLAYER_HEAD_BOTTOM,
		PLAYER_TORSO,
		PLAYER_TORSO_FRONT,
		PLAYER_TORSO_TOP,
		PLAYER_LEGS,
		PLAYER_LEGS_BOTTOM,
		PLAYER_ARM,
		SNOW,
		SNOW_SOIL_BORDER,
		PLANT_1,
		PLANT_2,
		SHEEP_HEAD_FRONT,
		SHEEP_HEAD,
		SHEEP_BODY,
	};

	void generate_face(int x, int y, int z, FaceDirection dir, const std::array<float, 12>& face_vertices, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f }, const glm::vec3& offset = { 0.0f, 0.0f, 0.0f });
	void generate_face_texture(BlockTexture texture);
	void generate_normals(const std::array<float, 3>& normals);

private:

;

	bool occluded(const BlockMap& height_map, int x, int y, int z) const;
	void generate_block(const BlockMap& height_map, int x, int y, int z, bool is_top_block);

	bool m_initialized;
};