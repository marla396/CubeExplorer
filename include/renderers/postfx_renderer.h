#pragma once

#include <memory>

#include "gl.h"
#include "camera.h"
#include "framebuffer.h"
#include "shaders/postfx/plain_shader.h"
#include "shaders/postfx/ssao_shader.h"
#include "shaders/postfx/lowpass_x_shader.h"
#include "shaders/postfx/underwater_shader.h"
#include "texture/mtexture.h"
#include "texture/ftexture.h"
#include "models/quad2d_model.h"

class PostFXRenderer {
public:
	PostFXRenderer();

	void render(const std::shared_ptr<FrameBuffer>& input_fbo, Camera& camera);

	void update(float time);

private:
	void lowpass_x(const std::shared_ptr<FrameBuffer>& input_fbo, const std::shared_ptr<FrameBuffer>& output_fbo);
	void underwater(const std::shared_ptr<ITexture>& input, const std::shared_ptr<ITexture>& output);

	void init_ssao();
	std::vector<float> generate_lowpass_kernel();
	glm::ivec3 get_works_groups() const;

	float m_time;


	std::unique_ptr<PlainShader> m_plain_shader;

	std::unique_ptr<UnderwaterShader> m_underwater_shader;
	std::unique_ptr<FTexture> m_underwater_texture;

	std::unique_ptr<SSAOShader> m_ssao_shader;
	std::unique_ptr<MTexture<float>> m_ssao_noise;

	std::unique_ptr<LowpassXShader> m_lowpass_x_shader;

	std::unique_ptr<Quad2DModel> m_quad;
	std::shared_ptr<FrameBuffer> m_pingpong_fbo;
};