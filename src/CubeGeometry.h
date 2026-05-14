#pragma once
class CubeGeometry
{
public:
	// Zakazanie copy konstruktoru a priradenia, kvoli singletonu
	// Riesenie problemu dvojteho uvolnenia v pripade:
	// Zle - CubeGeometry cube_mesh = CubeGeometry::GetCubeGeometry();
	// Dobre - CubeGeometry& cube_mesh = CubeGeometry::GetCubeGeometry();
	CubeGeometry(const CubeGeometry&) = delete;
	CubeGeometry& operator=(const CubeGeometry&) = delete;
	CubeGeometry();
	~CubeGeometry();
	static CubeGeometry& GetCubeGeometry();

	void Draw();

private:
	GLuint m_cube_vbo{ 0 };
	GLuint m_cube_vao{ 0 };
};