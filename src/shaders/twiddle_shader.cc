#include <algorithm>

#include "shaders/twiddle_shader.h"

#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

TwiddleShader::TwiddleShader() : Shader({ "twiddleshader.comp" }), TexShader(this) {
	bind();
	init_indices();

	GLC(glGenBuffers(1, &m_ssbo));
	GLC(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo));
	GLC(glBufferData(GL_SHADER_STORAGE_BUFFER, WATER_FFT_DIMENSION * sizeof(int32_t), reinterpret_cast<const void*>(m_indices.data()), GL_STATIC_READ));
	
	get_uniform_locations();
}

TwiddleShader::~TwiddleShader() {
	GLC(glDeleteBuffers(1, &m_ssbo));
}


void TwiddleShader::upload_N(int n) const {
	upload_uniform(m_N_location, n);
}

void TwiddleShader::bind_buffer_base(uint32_t index) const {
	GLC(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_ssbo));
}

void TwiddleShader::init_indices() {
	int32_t bits = static_cast<int32_t>(WATER_FFT_DIMENSION_LOG2);

	for (int32_t i = 0; i < static_cast<int32_t>(WATER_FFT_DIMENSION); i++) {
		
		int32_t x = reverse_bits(i);
		x = _rotl(x, bits);
		m_indices[i] = x;
	}
}

int32_t TwiddleShader::reverse_bits(int32_t n) const {
	n = ((n >> 1) & 0x55555555) | ((n << 1) & 0xaaaaaaaa);
	n = ((n >> 2) & 0x33333333) | ((n << 2) & 0xcccccccc);
	n = ((n >> 4) & 0x0f0f0f0f) | ((n << 4) & 0xf0f0f0f0);
	n = ((n >> 8) & 0x00ff00ff) | ((n << 8) & 0xff00ff00);
	n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
	return n;
}

void TwiddleShader::get_uniform_locations() {
	TexShader::get_uniform_locations();

	m_N_location = get_uniform_location("N");
}
