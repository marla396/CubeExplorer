#pragma once

#include "renderer.h"
#include "shaders/water_shader.h"
#include "shaders/h0k_shader.h"
#include "shaders/hkt_shader.h"
#include "shaders/twiddle_shader.h"
#include "shaders/butterfly_shader.h"
#include "shaders/inversion_shader.h"
#include "shaders/normal_shader.h"
#include "shaders/dudv_shader.h"
#include "models/water_model.h"
#include "texture/ftexture.h"
#include "texture/mtexture.h"
#include "texture/texture_storage.h"
#include "misc/white_noise_generator.h"
#include "world/world_const.h"
#include "world/world.h" 
#include "framebuffer.h"

using TexStorage = TextureStorage<float, WATER_FFT_DIMENSION, WATER_FFT_DIMENSION>;
using TexStoragePtr = std::shared_ptr<TexStorage>;
using TexStorageLog2 = TextureStorage<float, WATER_FFT_DIMENSION_LOG2, WATER_FFT_DIMENSION>;
using TexStorageLog2Ptr = std::shared_ptr<TexStorageLog2>;

class WaterRenderer : public Renderer<WaterShader, WaterModel> {
public:
	WaterRenderer(const std::shared_ptr<World>& world);

	void set_L(int L);
	void set_amplitude(float amplitude);
	void set_wind_speed(float wind_speed);
	void set_wind_direction(const glm::vec2& wind_direction);
	void set_capillar_supress_factor(float factor);
	void set_wave_strength(float strength);
	float get_wave_strength() const;

	void set_terrain_renderers(const std::vector<std::pair<std::function<void(Camera&)>, std::function<void(const glm::vec4&)>>>& renderers);

	TexStoragePtr get_tilde_h0k() const;
	TexStoragePtr get_tilde_h0minusk() const;
	TexStoragePtr get_tilde_hkt_dx() const;
	TexStoragePtr get_tilde_hkt_dy() const;
	TexStoragePtr get_tilde_hkt_dz() const;
	TexStoragePtr get_dx() const;
	TexStoragePtr get_dy() const;
	TexStoragePtr get_dz() const;
	TexStorageLog2Ptr get_twiddle_factors() const;
	TexStoragePtr get_pingpong() const;
	TexStoragePtr get_normal_map() const;
	TexStoragePtr get_dudv_map() const;
	std::shared_ptr<ITexture> get_reflection() const;
	std::shared_ptr<ITexture> get_refraction() const;

	void render(const std::vector<std::shared_ptr<WaterModel>>& models, Camera& camera, const std::shared_ptr<Light>& light);
	void render_depth(const std::vector<std::shared_ptr<WaterModel>>& models, Camera& camera, const std::shared_ptr<Light>& light, int cascade);
	void update(Camera& camera, float time);
	void compute_h0k() const;
	void compute_hkt() const;
	void compute_twiddle() const;
	void compute_fft() const;
	void render_reflection(Camera& camera);
	void render_refraction(Camera& camera);
private:
	void run_fft(int& pingpong) const;



	void initialize_h0k();
	void initialize_hkt();
	void initialize_twiddle();
	void initialize_butterfly();
	void initialize_inversion();

	WhiteNoiseGenerator m_generator;
	std::unique_ptr<WaterShader> m_depth_shader;

	std::unique_ptr<H0kShader> m_h0k_shader;
	TexStoragePtr m_tilde_h0k;
	TexStoragePtr m_tilde_h0minusk;

	std::unique_ptr<HktShader> m_hkt_shader;
	TexStoragePtr m_tilde_hkt_dx;
	TexStoragePtr m_tilde_hkt_dy;
	TexStoragePtr m_tilde_hkt_dz;

	TexStoragePtr m_dx;
	TexStoragePtr m_dy;
	TexStoragePtr m_dz;


	std::unique_ptr<TwiddleShader> m_twiddle_shader;
	TexStorageLog2Ptr m_twiddle_factors;
	
	std::unique_ptr<ButterflyShader> m_butterfly_shader;
	TexStoragePtr m_pingpong;

	std::unique_ptr<InversionShader> m_inversion_shader;
	std::unique_ptr<NormalShader> m_normal_shader;
	std::unique_ptr<DuDvShader> m_dudv_shader;

	TexStoragePtr m_normal_map;
	TexStoragePtr m_dudv_map;
	
	std::unique_ptr<MTexture<float>> m_noise_r0;
	std::unique_ptr<MTexture<float>> m_noise_i0;
	std::unique_ptr<MTexture<float>> m_noise_r1;
	std::unique_ptr<MTexture<float>> m_noise_i1;

	float m_time;
	int m_L;
	float m_amplitude;
	float m_wind_speed;
	glm::vec2 m_wind_direction;
	float m_capillar_supress_factor;
	float m_wave_strength;
	bool m_below_water;

	std::vector<std::pair<std::function<void(Camera&)>, std::function<void(const glm::vec4&)>>> m_terrain_renderers;

	std::shared_ptr<World> m_world;
	std::shared_ptr<FrameBuffer> m_reflection_fbo;
	std::shared_ptr<FrameBuffer> m_refraction_fbo;
};
