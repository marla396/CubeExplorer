#include "misc/white_noise_generator.h"

WhiteNoiseGenerator::WhiteNoiseGenerator(uint32_t seed) {
	set_seed(seed);
}

void WhiteNoiseGenerator::set_seed(uint32_t seed) {
	m_rng.seed(seed);
}
