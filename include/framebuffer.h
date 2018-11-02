#pragma once

#include <stack>

#include "gl.h"
#include "texture/itexture.h"
#include "application.h"

enum FrameBufferAttachment{
	FBO_TEXTURE = 1,
	FBO_RENDERBUFFER = 2,
	FBO_DEPTH_TEXTURE = 3
};


class FrameBuffer {
public:
	FrameBuffer(size_t width, size_t height, uint32_t attachments);
	~FrameBuffer();

	uint32_t get_id() const;
	size_t get_width() const;
	size_t get_height() const;

	void set_resolution(size_t width, size_t height);
	std::shared_ptr<ITexture> get_texture() const;

	void attach_texture();
	void attach_renderbuffer();
private:
	void create();
	void destroy();

	void bind() const;


	size_t m_width;
	size_t m_height;
	uint32_t m_attachments;

	uint32_t m_fbo;
	uint32_t m_rbo;
	std::shared_ptr<ITexture> m_texture;
	std::shared_ptr<ITexture> m_depth_texture;
	uint32_t m_render_buffer;
};

void bind_fbo(const std::shared_ptr<FrameBuffer>& fbo);
void unbind_fbo();

static void bind_top_fbo();