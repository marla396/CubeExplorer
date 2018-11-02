#pragma once

#include <memory>

#include "gl.h"
#include "framebuffer.h"
#include "shaders/postfx/plain_shader.h"
#include "shaders/postfx/lowpass_x_shader.h"
#include "models/quad2d_model.h"

class PostFXRenderer {
public:
	PostFXRenderer();

	void render(const std::shared_ptr<FrameBuffer>& input_fbo);

private:
	void lowpass_x(const std::shared_ptr<FrameBuffer>& input_fbo, const std::shared_ptr<FrameBuffer>& output_fbo);

	std::vector<float> generate_lowpass_kernel();

	std::unique_ptr<PlainShader> m_plain_shader;
	std::unique_ptr<LowpassXShader> m_lowpass_x_shader;

	std::unique_ptr<Quad2DModel> m_quad;
	std::shared_ptr<FrameBuffer> m_pingpong_fbo;
};