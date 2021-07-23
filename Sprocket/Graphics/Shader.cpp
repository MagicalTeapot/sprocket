#include "Shader.h"

#include <Sprocket/Utility/Log.h>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <format>
#include <fstream>
#include <string>

namespace spkt {

namespace {

// Parses a shader source code into a string ready to be compiled.
std::string parse_shader_source(std::string_view filepath)
{
	std::ifstream stream(std::string{filepath});
	if (!stream) {
		log::fatal("Shader file '{}' does not exist!", filepath);
	}
	std::string shader((std::istreambuf_iterator<char>(stream)),
		                std::istreambuf_iterator<char>());
	return shader;
}

std::uint32_t compile_shader_source(std::uint32_t type, const std::string& source)
{
	std::uint32_t id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE) {
        log::error("Could not compile shader {}", (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT");
		glDeleteShader(id);
		return 0;
	}

	return id;
}

}

shader::shader(
	std::string_view vert_shader_file,
	std::string_view frag_shader_file
)
	: d_vert_source(parse_shader_source(vert_shader_file))
	, d_frag_source(parse_shader_source(frag_shader_file))
	, d_program_id(glCreateProgram())
	, d_vert_shader_id(compile_shader_source(GL_VERTEX_SHADER, d_vert_source))
	, d_frag_shader_id(compile_shader_source(GL_FRAGMENT_SHADER, d_frag_source))
{
	glAttachShader(d_program_id, d_vert_shader_id);
	glAttachShader(d_program_id, d_frag_shader_id);
	glLinkProgram(d_program_id);
	glValidateProgram(d_program_id);
}

shader::~shader()
{
    unbind();
    glDetachShader(d_program_id, d_vert_shader_id);
    glDetachShader(d_program_id, d_frag_shader_id);
    glDeleteShader(d_vert_shader_id);
    glDeleteShader(d_frag_shader_id);
    glDeleteProgram(d_program_id);
}

void shader::bind() const
{
    glUseProgram(d_program_id);
}

void shader::unbind() const
{
    glUseProgram(0);
}

std::uint32_t shader::uniform_location(const std::string& name) const
{
	return glGetUniformLocation(d_program_id, name.c_str());
}

void shader::load(const std::string& name, int value) const
{
	glProgramUniform1i(d_program_id, uniform_location(name), value);
}

void shader::load(const std::string& name, float value) const
{
	glUniform1f(uniform_location(name), value);
}

void shader::load(const std::string& name, const glm::vec2& vector) const
{
	glUniform2f(uniform_location(name), vector.x, vector.y);
}

void shader::load(const std::string& name, const glm::vec3& vector) const
{
	glUniform3f(uniform_location(name), vector.x, vector.y, vector.z);
}

void shader::load(const std::string& name, const glm::vec4& vector) const
{
	glUniform4f(uniform_location(name), vector.x, vector.y, vector.z, vector.w);
}

void shader::load(const std::string& name, const glm::quat& quat) const
{
	glUniform4f(uniform_location(name), quat.x, quat.y, quat.z, quat.w);
}

void shader::load(const std::string& name, const glm::mat4& matrix, int count) const
{
	glUniformMatrix4fv(uniform_location(name), count, GL_FALSE, glm::value_ptr(matrix));
}

std::string array_name(std::string_view uniformName, std::size_t index)
{
	return std::format("{}[{}]", uniformName, index);
}

bool shader::reload()
{
	std::uint32_t programId = glCreateProgram();

	std::uint32_t vertShaderId = compile_shader_source(GL_VERTEX_SHADER, d_vert_source);
	if(vertShaderId == 0) {
		glDeleteProgram(programId);
		return false;
	}

	std::uint32_t fragShaderId = compile_shader_source(GL_FRAGMENT_SHADER, d_frag_source);
	if(fragShaderId == 0) {
		glDeleteShader(vertShaderId);
		glDeleteProgram(programId);
		return false;
	}

	glAttachShader(programId, vertShaderId);
	glAttachShader(programId, fragShaderId);
	glLinkProgram(programId);
	glValidateProgram(programId);

	GLint validateStatus = GL_FALSE;
	glGetProgramiv(programId, GL_VALIDATE_STATUS, &validateStatus);

	if(validateStatus != GL_TRUE) {
		glDeleteShader(fragShaderId);
		glDeleteShader(vertShaderId);
		glDeleteProgram(programId);
		return false;
	}

	glDeleteShader(d_vert_shader_id);
	glDeleteShader(d_frag_shader_id);
	glDeleteProgram(d_program_id);

	d_program_id = programId;
	d_vert_shader_id = vertShaderId;
	d_frag_shader_id = fragShaderId;

	return true;
}

}