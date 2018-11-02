#pragma once

#include <random>
#include <array>
#include <complex>

class WhiteNoiseGenerator {
public:
	WhiteNoiseGenerator(uint32_t seed);

	template<typename T, size_t W, size_t D> 
	std::array<std::array<T, D>, W> generate(T min, T max) {
		std::array<std::array<T, D>, W> ret;

		auto dist = get_distribution<T>(min, max);

		for (size_t x = 0; x < W; x++) {
			for (size_t z = 0; z < D; z++) {
				ret[x][z] = dist(m_rng);
			}
		}

		return ret;
	}

	void set_seed(uint32_t seed);
private:

	template<typename T>
	auto get_distribution(T min, T max) -> std::enable_if_t<std::is_integral<T>::value, std::uniform_int_distribution<T>> {
		return std::uniform_int_distribution<T>(min, max);
	}

	template<typename T>
	auto get_distribution(T min, T max) -> std::enable_if_t<std::is_floating_point<T>::value, std::uniform_real_distribution<T>> {
		return std::uniform_real_distribution<T>(min, max);
	}

	std::mt19937 m_rng;
};