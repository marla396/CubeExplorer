#include "world/block.h"

Block::Block(bool is_top_chunk) {
	m_is_top_chunk = is_top_chunk;
}

bool Block::is_top_chunk() const {
	return m_is_top_chunk;
}