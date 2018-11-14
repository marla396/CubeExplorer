#include "framebuffer.h"

FrameBuffer::FrameBuffer(size_t width, size_t height, uint32_t attachments) 
	: FrameBuffer(width, height, attachments, 1) {
}

FrameBuffer::FrameBuffer(size_t width, size_t height, uint32_t attachments, uint32_t n_textures) 
	: m_width(width), m_height(height), m_attachments(attachments), m_n_textures(n_textures) {
	create();
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

std::shared_ptr<ITexture> FrameBuffer::get_depth_texture() const {
	return m_depth_texture;
}

void FrameBuffer::attach_texture(uint32_t attachment){

	std::shared_ptr<ITexture> texture = std::make_shared<ITexture>();
	texture = std::make_shared<ITexture>();
	texture->bind();
	texture->buffer_data(m_width, m_height, static_cast<uint8_t*>(nullptr));
	texture->set_filter(GL_LINEAR);

	bind();
	GLC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, texture->get_id(), 0));
	
	m_textures.push_back(texture);

	CHECK_FRAME_BUFFER();
}

void FrameBuffer::attach_depth_texture() {
	bind();

	if (m_attachments & FBO_DEPTH_TEXTURE) {
		GLC(glDrawBuffer(GL_NONE));
		GLC(glReadBuffer(GL_NONE));
	}

	m_depth_texture = std::make_shared<ITexture>();
	m_depth_texture->bind();
	GLC(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
	m_depth_texture->set_filter(GL_LINEAR);

	GLC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture->get_id(), 0));


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

void FrameBuffer::create() {
	GLC(glGenFramebuffers(1, &m_fbo))

	if (m_attachments & FBO_TEXTURE) {
		for (uint32_t i = 0; i < m_n_textures; i++)
			attach_texture(i);
	}

	if (m_attachments & FBO_DEPTH_TEXTURE) {
		attach_depth_texture();
	}

	if (m_attachments & FBO_RENDERBUFFER) {
		attach_renderbuffer();
	}

/*	std::vector<GLenum> attachments;

	for (uint32_t i = 0; i < m_n_textures; i++) {
		attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
	}

	GLC(glDrawBuffers(m_n_textures, const_cast<GLenum*>(attachments.data())));*/
}

void FrameBuffer::destroy() {
	if (m_attachments & FBO_RENDERBUFFER) {
		GLC(glDeleteRenderbuffers(1, &m_rbo));
	}

	m_textures.clear();

	GLC(glDeleteFramebuffers(1, &m_fbo))
}


void FrameBuffer::bind() const {
	GLC(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));

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
		GLC(glBindFramebuffer(GL_FRAMEBUFFER, fbo->get_id()));
		GLC(glViewport(0, 0, fbo->get_width(), fbo->get_height()));
	}
}