#pragma once
#include <glm/glm.hpp>
#include <string_view>
#include "gl.h"


const std::string GLSL_VERSION = "450 core";

class Shader {
public:
	Shader(const std::initializer_list<std::string>& files);
	virtual ~Shader();

	Shader() = delete;
	Shader(const Shader&) = delete;
	Shader(const Shader&&) = delete;

	void bind() const;
	void unbind() const;

	void upload_uniform(int location, int i) const;
	void upload_uniform(int location, float f) const;

	void upload_uniform(int location, const glm::vec2& v) const;
	void upload_uniform(int location, const glm::vec3& v) const;
	void upload_uniform(int location, const glm::vec4& v) const;

	void upload_uniform(int location, const glm::mat3& m) const;
	void upload_uniform(int location, const glm::mat4& m) const;

	void upload_uniform(int location, const float* data, size_t count) const;

	//friend class MVPShader;
	//friend class TexShader;
	//friend class MultiTexShader; <-- will not work :(

	//Have to be public in order to handle templated extension shaders
	virtual void get_uniform_locations() = 0;
	int get_uniform_location(const std::string& name);

private:
	void compile(const std::string& file);
	bool preprocess_shader(const std::string& in, std::string& out) const;
	uint32_t get_shader_type(const std::string& file) const;
	bool compiler_error(const std::string& file, uint32_t s) const;

	uint32_t m_program;
};