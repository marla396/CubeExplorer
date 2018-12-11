#include "gl.h"

size_t draw_calls = 0;

void clear_OpenGL_errors() {
	while (glGetError() != GL_NO_ERROR);
}

bool check_OpenGL_errors(const char* function, const char* file, int line) {

	MAYBE_UNUSED(function);
	MAYBE_UNUSED(file);
	MAYBE_UNUSED(line);

	while (GLenum err = glGetError()) {
		MAYBE_UNUSED(err);
		LOG("OpenGL error [" << err << "] in " << file << "::" << function << " at line " << line);

		return false;
	}

	return true;
}

void check_framebuffer_status() {
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		LOG("Failed to create framebuffer" << std::endl);
	}
}

size_t get_draw_calls() {
	size_t tmp = draw_calls;
	draw_calls = 0;
	return tmp;
}

void increment_draw_calls() {
	++draw_calls;
}