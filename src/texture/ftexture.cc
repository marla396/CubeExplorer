#include "texture/ftexture.h"

FTexture::FTexture(const std::string& path, ITexture::MemoryLayout layout) : ITexture() {
	load_from_file(path, layout);
}
