#pragma once

#include "itexture.h"

class CubeTexture : public ITexture {
public:
	CubeTexture(size_t width, size_t height);
	void bind() const override;
	void bind(GLenum tex_unit) const override;
};