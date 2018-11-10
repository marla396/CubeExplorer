#include "framebuffer.h"

FrameBuffer::FrameBuffer(size_t width, size_t height, uint32_t attachments) 
	: m_width(width), m_height(height), m_attachments(attachments) {
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

std::shared_ptr<ITexture> FrameBuffer::get_texture() const {
	return m_texture;
}

std::shared_ptr<ITexture> FrameBuffer::get_depth_texture() const {
	return m_depth_texture;
}

void FrameBuffer::attach_texture(){

	m_texture = std::make_shared<ITexture>();
	m_texture->bind();
	m_texture->buffer_data(m_width, m_height, static_cast<float*>(nullptr));
	m_texture->set_filter(GL_LINEAR);

	bind();
	GLC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->get_id(), 0));

	CHECK_FRAME_BUFFER();
}

void FrameBuffer::attach_depth_texture() {
	bind();

	m_depth_texture = std::make_shared<ITexture>();
	m_depth_texture->bind();
	GLC(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
	m_depth_texture->set_filter(GL_NEAREST);
	//m_depth_texture->set_wrap(GL_CLAMP_TO_EDGE);

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
		attach_texture();
	}

	if (m_attachments & FBO_DEPTH_TEXTURE) {
		attach_depth_texture();
	}

	if (m_attachments & FBO_RENDERBUFFER) {
		attach_renderbuffer();
	}
}

void FrameBuffer::destroy() {
	if (m_attachments & FBO_RENDERBUFFER) {
		GLC(glDeleteRenderbuffers(1, &m_rbo));
	}

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