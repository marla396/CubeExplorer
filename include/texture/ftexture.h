#pragma once

#include "itexture.h"

class FTexture : public ITexture {
public:
	FTexture(const std::string& path, MemoryLayout layout = RGBA8UI);
};