#include "texture/cube_texture.h"

CubeTexture::CubeTexture(size_t width, size_t height) : ITexture() {
	m_width = width;
	m_height = height;
}

void CubeTexture::bind() const {
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex_id);
}

void CubeTexture::bind(GLenum tex_unit) const {
	glActiveTexture(tex_unit);
	bind();
}