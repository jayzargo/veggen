#pragma once
class SphereGeometry
{
public:
	SphereGeometry(const SphereGeometry&) = delete;
	SphereGeometry& operator=(const SphereGeometry&) = delete;
	SphereGeometry(int x_samples = 64, int y_samples = 64);
	~SphereGeometry();
	static SphereGeometry& GetSphereGeometry(int x_samples = 64, int y_samples = 64);

	void Draw();
private:
	GLuint m_sphere_vbo{ 0 };
	GLuint m_sphere_vao{ 0 };
	GLuint m_sphere_ebo{ 0 };
	size_t m_indices_cnt{ 0 };
};