#pragma once
#include "itexture.h"
#include <array>

template<size_t W, size_t H>
class TextureAtlas : public ITexture {
public:
	TextureAtlas(const std::string& path);
	std::array<float, 8> get_texture_coords(size_t texture_x, size_t texture_y) const override;
private:

	float m_texture_width;
	float m_texture_height;
};

template<size_t W, size_t H>
TextureAtlas<W, H>::TextureAtlas(const std::string& path)
	: ITexture() {
        load_from_file(path, ITexture::RGBA8UI);
        m_texture_width = static_cast<float>(m_width / W);
        m_texture_height = static_cast<float>(m_height / H);
}

template<size_t W, size_t H>
std::array<float, 8> TextureAtlas<W, H>::get_texture_coords(size_t texture_x, size_t texture_y) const {

	if (m_width < texture_x * m_texture_width || m_height < texture_y * m_texture_height) {
		LOG("Tried to access texture atlas outside of bounds (x: " << texture_x << ", y: " << texture_y << std::endl);
	}

	float fx = static_cast<float>(texture_x);
	float fy = static_cast<float>(texture_y);

	std::array<GLfloat, 8> ret = {
		(fx + 0.0f) / m_texture_width, (fy + 0.0f) / m_texture_height,
		(fx + 1.0f) / m_texture_width, (fy + 0.0f) / m_texture_height,
		(fx + 0.0f) / m_texture_width, (fy + 1.0f) / m_texture_height,
		(fx + 1.0f) / m_texture_width, (fy + 1.0f) / m_texture_height,
	};

	return ret;
}