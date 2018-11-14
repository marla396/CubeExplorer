#include "texture/itexture.h"
#include "nanovg/stb_image.h"
#include "resources.h"

ITexture::ITexture() : m_destroyed(false){
    GLC(glGenTextures(1, &m_tex_id));
}

ITexture::~ITexture(){
	destroy();
}

void ITexture::bind() const{
	GLC(glBindTexture(GL_TEXTURE_2D, m_tex_id));
}

uint32_t ITexture::get_id() const {
	return m_tex_id;
}

void ITexture::destroy() {
	if (!m_destroyed) {
		GLC(glDeleteTextures(1, &m_tex_id));
		m_destroyed = true;
	}
}

void ITexture::bind(GLenum texture_unit) const{
    GLC(glActiveTexture(texture_unit));   
    GLC(glBindTexture(GL_TEXTURE_2D, m_tex_id));
}

void ITexture::bind_image_texture(uint32_t texture_unit, GLenum access, GLenum format) const {
	GLC(glBindImageTexture(texture_unit, m_tex_id, 0, GL_FALSE, 0, access, format));
}

void ITexture::set_parameter(GLenum name, int parameter) const{
    GLC(glTexParameteri(GL_TEXTURE_2D, name, parameter));
}

void ITexture::set_filter(int filter) const {
	GLC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
	GLC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
}

void ITexture::set_wrap(int wrap) const {
	GLC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap));
	GLC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap));
}


void ITexture::load_from_file(const std::string& path, ITexture::MemoryLayout layout){
    stbi_set_flip_vertically_on_load(1);
	 
	stbi_uc* buffer = stbi_load(Resources::resolve_texture_path(path).c_str(), &m_width, &m_height, &m_bpp, 4);

    if (buffer){

		if (layout == RGBA32F) {
			
			float* fbuffer = new float[m_width * m_height * 4];

			for (int32_t x = 0; x < m_width; x++) {
				for (int32_t y = 0; y < m_height; y++) {

					uint8_t r = buffer[4 * (x + y * m_width) + 0];
					uint8_t g = buffer[4 * (x + y * m_width) + 1];
					uint8_t b = buffer[4 * (x + y * m_width) + 2];
					uint8_t a = buffer[4 * (x + y * m_width) + 3];

					fbuffer[4 * (x + y * m_width) + 0] = (static_cast<float>(r) / 255.0f);
					fbuffer[4 * (x + y * m_width) + 1] = (static_cast<float>(g) / 255.0f);
					fbuffer[4 * (x + y * m_width) + 2] = (static_cast<float>(b) / 255.0f);
					fbuffer[4 * (x + y * m_width) + 3] = (static_cast<float>(a) / 255.0f);
				}
			}

			load_from_memory(m_width, m_height, fbuffer);

			delete[] fbuffer;

		} else if (layout == RGBA8UI) {
			//already loaded as this format
			load_from_memory(m_width, m_height, buffer);
		}

		stbi_image_free(buffer);
    } else{
        LOG("Failed to load texture " << path << std::endl);
    }
}

void ITexture::buffer_data(size_t width, size_t height, const uint8_t* data) const {
	GLC(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
}

void ITexture::buffer_data(size_t width, size_t height, const uint32_t* data) const {
	GLC(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_INT, data));
}

void ITexture::buffer_data(size_t width, size_t height, const float* data) const {
	GLC(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data));
}

void ITexture::default_parameters() {
	GLC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
}