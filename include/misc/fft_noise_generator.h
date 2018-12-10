#pragma once

#include <random>
#include <complex>
#include <array>

#include <simple_fft/fft.hpp>
#include "fft_noise.h"
#include "world/world_const.h"

#include "gl.h"

class FFTNoiseGenerator {
public:
	FFTNoiseGenerator(uint32_t seed, float cutoff, float order, float gain)
		: m_cutoff(cutoff), m_order(order), m_gain(gain) {
		m_rng.seed(seed);
	}

	void set_cutoff(float fc) {
		m_cutoff = fc;
	}

	void set_order(float order) {
		m_order = order;
	}

	void set_gain(float gain) {
		m_gain = gain;
	}

	template<size_t W, size_t D>
	std::shared_ptr<FFTNoise<W, D>> generate_height_map(float min_height, float max_height) {

		auto ret = std::make_shared<FFTNoise<W, D>>();
		auto freq_domain = new std::complex<float>[W][D];

		std::uniform_real_distribution<float> dist(-max_height, max_height);

		//Generate white noise
		for (size_t x = 0; x < W; x++){
			for (size_t z = 0; z < D; z++) {
				std::complex<float> rnd(dist(m_rng), dist(m_rng));

				freq_domain[x][z] = rnd;
			}
		}

		float time_constant = 1.0f / (2.0f * PI * m_cutoff);

		//Filter the noise
		for (size_t x = 0; x < W; x++) {
			for (size_t z = 0; z < D; z++) { 
				
				float f = static_cast<float>(x) / static_cast<float>(W) + static_cast<float>(z) / static_cast<float>(D);
				
				freq_domain[x][z] *= lowpass_filter(f, time_constant, m_order, m_gain);
			}
		}

		const char *err;
		if (!simple_fft::IFFT(freq_domain, W, D, err)) {
			LOG("Failed to perform inverse fourier transform\n" << err << std::endl);
		}

		for (size_t x = 0; x < W; x++) {
			for (size_t z = 0; z < D; z++) {
				ret->noise[x][z] = std::abs(freq_domain[x][z]);
			}
		}

		float local_min = ret->noise[0][0];
		float local_max = local_min;
		ret->average = 0.0f;

		for (size_t x = 0; x < W; x++) {
			for (size_t z = 0; z < D; z++) {
				if (ret->noise[x][z] > local_max) {
					local_max = ret->noise[x][z];
				}

				if (ret->noise[x][z] < local_min) {
					local_min = ret->noise[x][z];
				}

				ret->average += ret->noise[x][z];
			}
		}

		ret->average /= static_cast<float>(W * D);

		//Normalise the noise to the specified min max values
		for (size_t x = 0; x < W; x++) {
			for (size_t z = 0; z < D; z++) {
				ret->noise[x][z] = (max_height - min_height) * ((ret->noise[x][z] - local_min) / (local_max - local_min)) + min_height;
			}
		}

		ret->max = max_height;
		ret->min = min_height;

		delete[] freq_domain;

		return ret;
	}
private:

	float lowpass_filter(float f, float time_constant, float order, float gain){


		return gain / std::pow(2.0f * PI * f * time_constant + 1.0f, order);
	}

	float highpass_filter(float f, float time_constant, float order, float gain){
		return gain * std::pow((time_constant * 2.0f * PI * f) / (1 + time_constant * 2.0f * PI * f), order);
	}

	float bandpass_filter(float f, float time_constant_l, float time_constant_h, float order, float gain){

		return lowpass_filter(f, time_constant_h, order, gain) * highpass_filter(f, time_constant_l, order, 1.0f);

	}

	float bandstop_filter(float f, float cutoff_l, float cutoff_h, float order, float gain){

		float wl = 1.0f / (2.0f * PI * cutoff_l);
		float wh = 1.0f / (2.0f * PI * cutoff_h);
		float wc = wh - wl;
		float w0 = wc / 2.0f + wl;


		float s2 = -f * f;
		float w02 = w0 * w0;

		return gain * std::pow((s2 + w02) / (s2 + 2.0f * wc + w02), order);
	}

	std::mt19937 m_rng;

	float m_cutoff;
	float m_order;
	float m_gain;
};