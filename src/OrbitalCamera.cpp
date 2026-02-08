#include "pch.h"
#include "OrbitalCamera.h"

OrbitalCamera::OrbitalCamera(const float yaw, const float pitch, const float radius, const glm::vec3 &center, const glm::vec3 &up)
{
	m_yaw = yaw;
	m_pitch = pitch;
	m_center = center;
	m_up = up;
	m_radius = radius;
	UpdateVectors();
}

glm::mat4 OrbitalCamera::GetViewMatrix()
{
	return glm::lookAt(m_eye, m_center, m_up);
}

void OrbitalCamera::UpdateVectors()
{
	glm::vec3 view_vector;
	view_vector.x = m_radius * cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
	view_vector.y = m_radius * sin(glm::radians(m_pitch));
	view_vector.z = m_radius * cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_eye = m_eye + m_center;
}
