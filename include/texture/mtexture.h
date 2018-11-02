#pragma once

#include "itexture.h"

template<typename T>
class MTexture : public ITexture {
public:
	MTexture(size_t width, size_t height, const T* data) {
		m_width = width;
		m_height = height;
		load_from_memory(width, height, data);
	}

	~MTexture() {

	}
};