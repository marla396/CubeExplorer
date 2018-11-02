#pragma once

#include <unordered_map>
#include "model.h"
#include "world/block_map.h"


class ChunkModel : public Model {
public:
	ChunkModel(const std::shared_ptr<ITexture>& texture, const glm::vec3& position, const BlockMap& block_map);
	~ChunkModel();

	void generate_geometry(const BlockMap& block_map);

	BlockMap get_block_map() const;
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

	bool ocluded(const BlockMap& height_map, int x, int y, int z) const;
	void generate_block(const BlockMap& height_map, int x, int y, int z, bool is_top_block);
	void generate_face(int x, int y, int z, FaceDirection dir, const std::array<float, 12>& face_vertices);
	void generate_face_texture(BlockTexture texture);

	BlockMap m_block_map;
};