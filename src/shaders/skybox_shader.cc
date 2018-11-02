#include "shaders/skybox_shader.h"

SkyboxShader::SkyboxShader() : Shader({ "skyboxshader.vert", "skyboxshader.frag" }), MVPShader(this), TexShader(this){
	bind();
	get_uniform_locations();
}

void SkyboxShader::get_uniform_locations() {
	MVPShader::get_uniform_locations();
	TexShader::get_uniform_locations();
}