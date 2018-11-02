#pragma once

#include "itexture.h"

template<typename T, size_t W, size_t H>
class TextureStorage : public ITexture {
public:
	TextureStorage(int level = 1) : ITexture() {
		bind();
		buffer_storage(level);
	}
private:

	/*auto buffer_storage(int level) -> std::enable_if_t<std::is_same<uint8_t, T>::value>{
		GLC(glTexStorage2D(GL_TEXTURE_2D, level, GL_RGBA8, W, H));
	}*/

	auto buffer_storage(int level) -> std::enable_if_t<std::is_same<float, T>::value> {
		GLC(glTexStorage2D(GL_TEXTURE_2D, level, GL_RGBA32F, W, H));
	}
};