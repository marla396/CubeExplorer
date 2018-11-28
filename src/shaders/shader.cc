
#include "shaders/shader.h"
#include "resources.h"
#include "world/light.h"

#include <glm/gtc/type_ptr.hpp>
#include <regex>

Shader::Shader(const std::initializer_list<std::string>& files) {

	GLC(m_program = glCreateProgram());

	for (const auto& file : files) {
		compile(Resources::resolve_shader_path(file));
	}

	GLC(glLinkProgram(m_program));

	check_linker_errors(files);
}	

Shader::~Shader() {
	GLC(glDeleteShader(m_program));
}

void Shader::bind() const{
	GLC(glUseProgram(m_program));
}	

void Shader::unbind() const{
	GLC(glUseProgram(0));
}

void Shader::upload_uniform(int location, int i) const {
	GLC(glUniform1i(location, i));
}

void Shader::upload_uniform(int location, float f) const {
	GLC(glUniform1f(location, f));
}

void Shader::upload_uniform(int location, const glm::vec2& v) const {
	GLC(glUniform2fv(location, 1, glm::value_ptr(v)));
}

void Shader::upload_uniform(int location, const glm::vec3& v) const {
	GLC(glUniform3fv(location, 1, glm::value_ptr(v)));
}

void Shader::upload_uniform(int location, const glm::vec4& v) const {
	GLC(glUniform4fv(location, 1, glm::value_ptr(v)));
}

void Shader::upload_uniform(int location, const glm::mat3& m) const {
	GLC(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(m)));
}

void Shader::upload_uniform(int location, const glm::mat4& m) const{
	GLC(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m)));
}

void Shader::upload_uniform(int location, const glm::ivec2& v) const {
	GLC(glUniform2iv(location, 1, glm::value_ptr(v)));
}

void Shader::upload_uniform(int location, const float* data, size_t count) const {
	GLC(glUniform1fv(location, count, data));
}

void Shader::upload_uniform(int location, const std::vector<glm::vec3>& data) const {
	GLC(glUniform3fv(location, data.size(), reinterpret_cast<float*>(const_cast<glm::vec3*>(data.data()))));
}

int Shader::get_uniform_location(const std::string& name) {

	GLC(int location = glGetUniformLocation(m_program, name.c_str()));
	
	if (location == -1) {
		LOG("Failed to get uniform location " << name << std::endl);
	}

	return location;
}


void Shader::compile(const std::string& file) {

	uint32_t shader_type = get_shader_type(file);
	std::string raw_source = Resources::read_file(file);
	std::string source;

	if (!preprocess_shader(raw_source, source)) {
		LOG("Failed to preprocess " << file);
		return;
	}
	const char* tmp = source.c_str();

	GLC(uint32_t s = glCreateShader(shader_type));
	GLC(glShaderSource(s, 1, &tmp, nullptr));
	GLC(glCompileShader(s));

	if (compiler_error(file, s)) {
		return;
	}

	GLC(glAttachShader(m_program, s));
	GLC(glDeleteShader(s));
}

bool Shader::preprocess_shader(std::string& in, std::string& out) const{

#ifdef _WIN32
	std::regex include("^#include [\"<](.+?)[>\"]$", std::regex_constants::ECMAScript);
#else
	std::regex include("#include [\"<](.+?)[>\"]");
#endif

	std::smatch match;
	std::string tmp = in;

	while (std::regex_search(tmp, match, include)) {

		for (int i = 1; i < match.size(); i++) {

			std::string ext = Resources::read_file(Resources::resolve_shader_path(match[i].str()));

			if (ext == "") {
				return false;
			}
			in = std::regex_replace(in, std::regex(match[0].str()), ext, std::regex_constants::format_first_only);
			tmp = match.suffix();
		}
	}

	out = "#define LIGHT_HIGH_DISTANCE " + std::to_string(Light::LIGHT_HIGH_DISTANCE / 2 - 5.0f) + "\n" + in;
	out = "#define SHADOW_EPSILON_HIGH 0.001\n#define SHADOW_EPSILON_LOW 0.005\n" + out;
	out = "#define M_PI 3.1415926535897932384626433832795\n" + out;
	out = "#version " + GLSL_VERSION + "\n" + out;
	
	return true;
}

uint32_t Shader::get_shader_type(const std::string& file) const{
	size_t extension_index = file.find_last_of(".");

	if (extension_index != std::string::npos) {

		std::string file_extension = file.substr(extension_index + 1);

		if (file_extension == "vert") {
			return GL_VERTEX_SHADER;
		}
		else if (file_extension == "frag") {
			return GL_FRAGMENT_SHADER;
		}
		else if (file_extension == "geom") {
			return GL_GEOMETRY_SHADER;
		}
		else if (file_extension == "tcs") {
			return GL_TESS_CONTROL_SHADER;
		}
		else if (file_extension == "tes") {
			return GL_TESS_EVALUATION_SHADER;
		}
		else if (file_extension == "comp") {
			return GL_COMPUTE_SHADER;
		}
	}

	LOG("Invalid shader extension in " << file << std::endl);

	return 0;
}

bool Shader::compiler_error(const std::string& file, uint32_t s) const{
	int log_length;

	GLC(glGetShaderiv(s, GL_INFO_LOG_LENGTH, &log_length));

	if (log_length > 2) {
		std::string compile_error(log_length, '\0');

		int written;
		GLC(glGetShaderInfoLog(s, log_length, &written, const_cast<char*>(compile_error.data())));

		LOG("Error compiling shader " << file << "\n" << compile_error << std::endl);

		return true;
	}

	return false;
}

void Shader::check_linker_errors(const std::initializer_list<std::string>& files) const {
	int link_ok = 0;
	GLC(glGetProgramiv(m_program, GL_LINK_STATUS, &link_ok));

	if (!link_ok) {
		int max_length = 0;
		GLC(glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &max_length));

		std::string error_msg(max_length, '\0');

		GLC(glGetProgramInfoLog(m_program, max_length, &max_length, error_msg.data()));

		LOG("Error linking shader ");
		for (const auto& f : files) {
			LOG(f << " ");
		}
		LOG("\n" << error_msg << std::endl);
	}
}