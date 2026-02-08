#pragma once
class OrbitalCamera
{
private:
	float m_yaw;
	float m_pitch;
	float m_sensitivity;
	float m_radius;

	glm::vec3 m_up;
	glm::vec3 m_center;
	glm::vec3 m_eye;

public:

	OrbitalCamera(const float yaw, const float pitch, const float radius, const glm::vec3 &center,const glm::vec3 &up);
	glm::mat4 GetViewMatrix();

private:
	void UpdateVectors();
};

