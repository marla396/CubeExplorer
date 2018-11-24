#include "framebuffer.h"

FrameBuffer::FrameBuffer(size_t width, size_t height, uint32_t attachments) 
	: m_width(width), m_height(height), m_attachments(attachments) {

	create();
}

void FrameBuffer::bind() const {
	GLC(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
}

FrameBuffer::~FrameBuffer() {
	destroy();
}

uint32_t FrameBuffer::get_id() const {
	return m_fbo;
}

size_t FrameBuffer::get_width() const {
	return m_width;
}

size_t FrameBuffer::get_height() const {
	return m_height;
}

void FrameBuffer::set_resolution(size_t width, size_t height) {
	m_width = width;
	m_height = height;

	destroy();
	create();

	bind_top_fbo();
}

std::shared_ptr<ITexture> FrameBuffer::get_texture(uint32_t n) const {
	return m_textures[n];
}

std::shared_ptr<ITexture> FrameBuffer::get_depth_texture(uint32_t n) const {
	return m_depth_textures[n];
}

void FrameBuffer::attach_texture(GLenum attachment){

	std::shared_ptr<ITexture> texture = std::make_shared<ITexture>();
	texture = std::make_shared<ITexture>();
	texture->bind();
	texture->buffer_data(m_width, m_height, static_cast<float*>(nullptr));
	texture->set_filter(GL_LINEAR);

	bind();
	GLC(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->get_id(), 0));
	
	m_textures.push_back(texture);

	CHECK_FRAME_BUFFER();
}

void FrameBuffer::attach_depth_texture() {
	bind();


	if (m_attachments == (FBO_DEPTH_TEXTURE | FBO_DEPTH_TEXTURE2) && m_attachments == FBO_DEPTH_TEXTURE) {
		GLC(glDrawBuffer(GL_NONE));
		GLC(glReadBuffer(GL_NONE));
	}
	

	std::shared_ptr<ITexture> texture = std::make_shared<ITexture>();
	texture->bind();
	GLC(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
	texture->set_filter(GL_LINEAR);
	texture->set_wrap(GL_CLAMP_TO_EDGE);

	GLC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->get_id(), 0));

	m_depth_textures.push_back(texture);

	CHECK_FRAME_BUFFER();
}

void FrameBuffer::attach_renderbuffer() {
	bind();
	GLC(glGenRenderbuffers(1, &m_rbo));
	GLC(glBindRenderbuffer(GL_RENDERBUFFER, m_rbo));
	GLC(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height));
	GLC(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo));

	CHECK_FRAME_BUFFER();
}

void FrameBuffer::bind_depth_texture(uint32_t n) {
	GLC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_textures[n]->get_id(), 0));
}

void FrameBuffer::create() {
	GLC(glGenFramebuffers(1, &m_fbo))

	if (m_attachments & FBO_TEXTURE) {
		attach_texture(GL_COLOR_ATTACHMENT0);
	}

	if (m_attachments & FBO_DEPTH_TEXTURE) {
		attach_depth_texture();
	}

	if (m_attachments & FBO_RENDERBUFFER) {
		attach_renderbuffer();
	}

	if (m_attachments & FBO_DEPTH_TEXTURE2) {
		attach_depth_texture();
	}
}

void FrameBuffer::destroy() {
	if (m_attachments & FBO_RENDERBUFFER) {
		GLC(glDeleteRenderbuffers(1, &m_rbo));
	}

	m_textures.clear();
	m_depth_textures.clear();
	m_draw_buffers.clear();

	GLC(glDeleteFramebuffers(1, &m_fbo))
}


std::stack<std::shared_ptr<FrameBuffer>> fbo_bind_stack;

void bind_fbo(const std::shared_ptr<FrameBuffer>& fbo) {
	GLC(glBindFramebuffer(GL_FRAMEBUFFER, fbo->get_id()));
	GLC(glViewport(0, 0, fbo->get_width(), fbo->get_height()));

	fbo_bind_stack.push(fbo);
}

void unbind_fbo() {
	fbo_bind_stack.pop();
	bind_top_fbo();
}

void bind_top_fbo() {

	if (fbo_bind_stack.empty()) {
		GLC(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		Application::update_viewport();
	}
	else {
		auto fbo = fbo_bind_stack.top();
		fbo->bind();
		GLC(glViewport(0, 0, fbo->get_width(), fbo->get_height()));
	}
}
