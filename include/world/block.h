#pragma once

#include "world_const.h"

class Block {
public:
	Block(bool is_top_chunk);

	bool is_top_chunk() const;
private:
	bool m_is_top_chunk;
};