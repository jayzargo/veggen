#pragma once
class CubeGeometry
{
public:
	CubeGeometry();
	~CubeGeometry();
	static CubeGeometry& GetCubeGeometry();

	void Draw();

private:
	GLuint m_cube_vbo{ 0 };
	GLuint m_cube_vao{ 0 };
};