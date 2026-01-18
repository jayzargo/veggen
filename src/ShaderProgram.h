#pragma once
// Bude v PCH

class ShaderProgram
{
public:
	ShaderProgram(const std::vector<std::string> shader_path_list);
	~ShaderProgram();

	void use();
	GLuint GetProgramID();

	void UniformSetInt(GLint value, const char* int_name);
	void UniformSetBool(const bool value, const char* bool_name);
	void UniformSetFloat(const GLfloat value, const char* float_name);

	void UniformSetSampler(GLenum texture_unit, const char* sampler_name);
	void UniformSetMatrix4x4(const GLfloat* data, const char* matrix_name);
	void UniformSetVector3(const GLfloat* data, const char* vector_name);
	void UniformSetVector2(const GLfloat* data, const char* vector_name);

private:
	GLuint m_program_id;
	std::array<GLuint, 3> m_shader_types = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER};

private:

	int LoadShader(const std::string& file_name, std::vector<char>& shader);
	GLint CheckShader(const GLenum shader);
	GLint CheckProgramLinking();
};

