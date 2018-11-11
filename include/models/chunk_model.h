#pragma once

#include <unordered_map>
#include "model.h"
#include "world/block_map.h"


class ChunkModel : public Model {
public:
	ChunkModel(const std::shared_ptr<ITexture>& texture, const glm::vec3& position, const BlockMap& block_map);
	~ChunkModel();

	void gpu_init();
	bool is_initialized() const;
	void generate_geometry(const BlockMap& block_map);

	BlockMap get_block_map() const;

	glm::vec3 get_center() const override;
	float get_bounding_radius() const override;
private:

	enum FaceDirection {
		POSITIVE,
		NEGATIVE
	};

	enum BlockTexture {
		GRASS,
		GRASS_SOIL_BORDER,
		SOIL,
		SAND
	};

	bool occluded(const BlockMap& height_map, int x, int y, int z) const;
	void generate_block(const BlockMap& height_map, int x, int y, int z, bool is_top_block);
	void generate_face(int x, int y, int z, FaceDirection dir, const std::array<float, 12>& face_vertices);
	void generate_face_texture(BlockTexture texture);
	void generate_normals(const std::array<float, 3>& normals);

	BlockMap m_block_map;
	bool m_initialized;
};