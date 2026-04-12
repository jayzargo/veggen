#include "pch.h"
#include "OrbitalCamera.h"

OrbitalCamera::OrbitalCamera(const float yaw, const float pitch, const float radius, const glm::vec3 &center, const glm::vec3 &up)
{
	m_yaw = yaw;
	m_pitch = pitch;
	m_center = center;
	m_up = up;
	m_radius = radius;
	m_sensitivity = 0.1;
	UpdateVectors();
}

glm::mat4 OrbitalCamera::GetViewMatrix()
{
	return glm::lookAt(m_eye, m_center, m_up);
}

void OrbitalCamera::ProcessMouseMovement(const float xoffset, const float yoffset)
{
	const float scaled_x = xoffset * m_sensitivity;
	const float scaled_y = yoffset * m_sensitivity;

	m_yaw += scaled_x;
	m_pitch += scaled_y;

	m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

	UpdateVectors();
}

void OrbitalCamera::UpdateVectors()
{
	m_eye.x = m_radius * cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
	m_eye.y = m_radius * sin(glm::radians(m_pitch));
	m_eye.z = m_radius * cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_eye = m_eye + m_center;
}
