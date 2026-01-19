#include "pch.h"
#include <utils.h>
#include <ShaderProgram.h>

ShaderProgram::ShaderProgram(const std::vector<std::string> &shader_path_list)
{
	m_program_id = glCreateProgram();

	for (int i = 0; i < shader_path_list.size(); i++) 
	{
		GLuint gl_shader_type = m_shader_types[i];
		GLuint shader = glCreateShader(gl_shader_type);
		std::vector<char> shader_source;

		if (LoadShader(shader_path_list[i], shader_source) == S_OK)
		{
			const char* tmp = static_cast<const char*>(&shader_source[0]);
			glShaderSource(shader, 1, &tmp, nullptr);
			glCompileShader(shader);
		}
		CheckShader(shader);

		glAttachShader(m_program_id, shader);
		glDeleteShader(shader);

	}

	// Link compiled shaders
	glLinkProgram(m_program_id);

	CheckProgramLinking();
}

int ShaderProgram::LoadShader(const std::string& file_name, std::vector<char>& shader)
{
	FILE* file = fopen(file_name.c_str(), "rb");

	if (!file)
	{
		printf("IO error: File '%s' not found.\n", file_name.c_str());

		return S_FALSE;
	}

	int result = S_FALSE;

	const size_t file_size = static_cast<size_t>(GetFileSize64(file_name.c_str()));

	if (file_size < 1)
	{
		printf("Shader error: File '%s' is empty.\n", file_name.c_str());
	}
	else
	{
		/* in glShaderSource we don't set the length in the last parameter,
		so the string must be null terminated, therefore +1 and reset to 0 */
		shader.clear();
		shader.resize(file_size + 1);

		size_t bytes = 0; // number of already loaded bytes

		do
		{
			bytes += fread(shader.data(), sizeof(char), file_size, file);
		} while (!feof(file) && (bytes < file_size));

		if (!feof(file) && (bytes != file_size))
		{
			printf("IO error: Unexpected end of file '%s' encountered.\n", file_name.c_str());
		}
		else
		{
			printf("Shader file '%s' loaded successfully.\n", file_name.c_str());
			result = S_OK;
		}
	}

	fclose(file);
	file = nullptr;

	return result;
}


void ShaderProgram::use()
{
	glUseProgram(m_program_id);
}

GLuint ShaderProgram::GetProgramID() 
{
	return m_program_id;
}

GLint ShaderProgram::CheckShader(const GLenum shader)
{
	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	printf("Shader compilation %s.\n", (status == GL_TRUE) ? "was successful" : "FAILED");

	if (status == GL_FALSE)
	{
		int info_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
		char* info_log = new char[info_length];
		memset(info_log, 0, sizeof(*info_log) * info_length);
		glGetShaderInfoLog(shader, info_length, &info_length, info_log);

		printf("Error log: %s\n", info_log);

		SAFE_DELETE_ARRAY(info_log);
	}

	return status;
}

GLint ShaderProgram::CheckProgramLinking()
{
	GLint status = 0;

	// Param status - status
	glGetProgramiv(m_program_id, GL_LINK_STATUS, &status);

	printf("Program linking %s.\n", (status == GL_TRUE) ? "was successful" : "FAILED");

	if (status == GL_FALSE)
	{
		int info_length = 0;

		// Param info length - konkretna dlzka infa - ak nechceme len hardcoded 512/1024 buffer
		glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &info_length);
		char* info_log = new char[info_length];
		memset(info_log, 0, sizeof(*info_log) * info_length);
		glGetProgramInfoLog(m_program_id, info_length, &info_length, info_log);

		printf("Error log: %s\n", info_log);

		SAFE_DELETE_ARRAY(info_log);
	}

	return status;
}

void ShaderProgram::UniformSetInt(GLint value, const char* int_name)
{
	const GLint location = glGetUniformLocation(m_program_id, int_name);

	if (location == -1)
	{
		printf("Integer value '%s' not found in active shader.\n", int_name);
	}
	else
	{
		glUniform1i(location, value);
	}
}

void ShaderProgram::UniformSetBool(const bool value, const char* bool_name)
{
	const GLint location = glGetUniformLocation(m_program_id, bool_name);

	if (location == -1)
	{
		printf("Bool value '%s' not found in active shader.\n", bool_name);
	}
	else
	{
		glUniform1i(location, value);
	}
}

void ShaderProgram::UniformSetFloat(const GLfloat value, const char* float_name)
{
	const GLint location = glGetUniformLocation(m_program_id, float_name);

	if (location == -1)
	{
		printf("Float value '%s' not found in active shader.\n", float_name);
	}
	else
	{
		glUniform1f(location, value);
	}
}

void ShaderProgram::UniformSetSampler(GLenum texture_unit, const char* sampler_name)
{
	const GLint location = glGetUniformLocation(m_program_id, sampler_name);

	if (location == -1)
	{
		printf("Texture sampler '%s' not found in active shader.\n", sampler_name);
	}
	else
	{
		glUniform1i(location, texture_unit);
	}
}

void ShaderProgram::UniformSetMatrix4x4(const GLfloat* data, const char* matrix_name)
{
	const GLint location = glGetUniformLocation(m_program_id, matrix_name);

	if (location == -1)
	{
		printf("Matrix '%s' not found in active shader.\n", matrix_name);
	}
	else
	{
		// Pri column/row order! pri glm nastavit GL_FALSE!
		glUniformMatrix4fv(location, 1, GL_FALSE, data);
	}
}

void ShaderProgram::UniformSetVector3(const GLfloat* data, const char* vector_name)
{
	const GLint location = glGetUniformLocation(m_program_id, vector_name);

	if (location == -1)
	{
		printf("Vector '%s' not found in active shader.\n", vector_name);
	}
	else
	{
		glUniform3fv(location, 1, data);
	}
}

void ShaderProgram::UniformSetVector2(const GLfloat* data, const char* vector_name)
{
	const GLint location = glGetUniformLocation(m_program_id, vector_name);

	if (location == -1)
	{
		printf("Vector '%s' not found in active shader.\n", vector_name);
	}
	else
	{
		glUniform2fv(location, 1, data);
	}
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_program_id);
}
