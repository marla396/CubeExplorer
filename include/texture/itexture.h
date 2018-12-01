#pragma once

#include <array>
#include "gl.h"

class ITexture {
public:
	enum MemoryLayout {
		RGBA8UI,
		RGBA32F
	};

	ITexture();
	virtual ~ITexture();

	uint32_t get_id() const;

	void destroy();

	virtual void bind() const;
	virtual void bind(GLenum texture_unit) const;
	void bind_image_texture(uint32_t texture_unit, GLenum access, GLenum format) const;
	void set_parameter(GLenum name, int parameter) const;
	void set_filter(int filter) const;
	void set_wrap(int wrap) const;
	void set_compare_mode(GLint compare_mode) const;
	void set_compare_func(GLint compare_func) const;

	virtual std::array<float, 8> get_texture_coords(size_t, size_t) const { return std::array<float, 8>(); }

	template<typename T>
	void load_from_memory(size_t width, size_t height, T* data) {

		GLC(glBindTexture(GL_TEXTURE_2D, m_tex_id));

		default_parameters();

		buffer_data(width, height, data);
	}

	//Requires specialization for some odd reason
	void load_from_memory(size_t width, size_t height, const float* data) {
		GLC(glBindTexture(GL_TEXTURE_2D, m_tex_id));

		default_parameters();

		buffer_data(width, height, data);
	}

	void load_from_file(const std::string& path, ITexture::MemoryLayout layout);

	void buffer_data(size_t width, size_t height, const uint8_t* data) const;
	void buffer_data(size_t width, size_t height, const uint32_t* data) const;
	void buffer_data(size_t width, size_t height, const float* data) const;

protected:
	int32_t m_width;
	int32_t m_height;
	int32_t m_bpp;
	uint32_t m_tex_id;
private:
	void default_parameters();
	
	bool m_destroyed;
};