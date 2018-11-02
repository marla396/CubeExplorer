#pragma once

#include <array>

#include "shader.h"
#include "tex_shader.h"
#include "world/world_const.h"

class TwiddleShader : public Shader, public TexShader{
public:
	TwiddleShader();
	~TwiddleShader();

	TwiddleShader(const TwiddleShader&) = delete;
	TwiddleShader(const TwiddleShader&&) = delete;

	void upload_N(int n) const;
	void bind_buffer_base(uint32_t index) const;
private:
	void init_indices();
	int32_t reverse_bits(int32_t i) const;
	void get_uniform_locations() override;

	std::array<int32_t, WATER_FFT_DIMENSION> m_indices;

	uint32_t m_ssbo;
	int m_N_location;
};