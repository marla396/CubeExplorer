#include "shaders/postfx/fxaa_shader.h"

FXAAShader::FXAAShader() : Shader({"postfx/fxaashader.comp"}), MultiTexShader(this){
	bind();
	get_uniform_locations();
}

void FXAAShader::upload_threshold(float threshold) const{
	upload_uniform(m_threshold_location, threshold);
}

void FXAAShader::get_uniform_locations(){
	MultiTexShader::get_uniform_locations();

	m_threshold_location = get_uniform_location("luma_threshold");
}
