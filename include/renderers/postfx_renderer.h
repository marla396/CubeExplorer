#pragma once

#include <memory>

#include "gl.h"
#include "camera.h"
#include "framebuffer.h"
#include "shaders/postfx/plain_shader.h"
#include "shaders/postfx/ssao_shader.h"
#include "shaders/postfx/ssao_merge_shader.h"
#include "shaders/postfx/ssao_blur_shader.h"
#include "shaders/postfx/underwater_shader.h"
#include "shaders/postfx/fxaa_shader.h"
#include "texture/mtexture.h"
#include "texture/ftexture.h"
#include "models/quad2d_model.h"

class PostFXRenderer {
public:
	PostFXRenderer();

	void render(const std::shared_ptr<FrameBuffer>& input_fbo, Camera& camera);

	void update(float time);
	void toggle_fxaa();
	void toggle_ssao();
private:
	void underwater(const std::shared_ptr<ITexture>& input, const std::shared_ptr<ITexture>& output, float depth);
	void fxaa(const std::shared_ptr<ITexture>& input, const std::shared_ptr<ITexture>& output);
	
	void init_ssao();
	std::vector<float> generate_lowpass_kernel();
	glm::ivec3 get_work_groups(int scale = 1) const;

	float m_time;
	bool m_fxaa;
	bool m_ssao;


	std::unique_ptr<PlainShader> m_plain_shader;

	std::unique_ptr<UnderwaterShader> m_underwater_shader;
	std::unique_ptr<FTexture> m_underwater_texture;

	std::unique_ptr<SSAOShader> m_ssao_shader;
	std::unique_ptr<SSAOMergeShader> m_ssao_merge_shader;
	std::unique_ptr<SSAOBlurShader> m_ssao_blur_shader;

	std::unique_ptr<FXAAShader> m_fxaa_shader;

	std::unique_ptr<Quad2DModel> m_quad;
	std::shared_ptr<FrameBuffer> m_ssao_fbo;
	std::shared_ptr<MTexture<float>> m_ssao_pingpong_texture;
	std::shared_ptr<MTexture<float>> m_pingpong_texture;
};
