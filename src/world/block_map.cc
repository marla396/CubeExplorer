#include "world/block_map.h"



std::shared_ptr<Block> BlockMap::operator()(int x, int y, int z) const {
	return m_blocks[x][y][z];
}

int BlockMap::get_global_height() const {
	return m_global_height;
}

void BlockMap::set_global_height(int height) {
	m_global_height = height;
}


void BlockMap::insert_block(int x, int y, int z, bool is_top_chunk) {
	m_blocks[x][y][z] = std::make_shared<Block>(is_top_chunk);
}