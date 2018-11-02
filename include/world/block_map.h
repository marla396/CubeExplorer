#pragma once

#include <array>
#include <memory>

#include "world_const.h"
#include "block.h"

class BlockMap {
public:
	BlockMap() = default;

	std::shared_ptr<Block> operator()(int x, int y, int z) const;

	int get_global_height() const;
	void set_global_height(int height);

	void insert_block(int x, int y, int z, bool is_top_chunk);
private:
	std::array<std::array<std::array<std::shared_ptr<Block>, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE> m_blocks {};
	int m_global_height;
};