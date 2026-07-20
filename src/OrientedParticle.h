#pragma once
#include "pch.h"

class OrientedParticle
{
public:
	OrientedParticle();
	~OrientedParticle();

	void init();

public:
	// mass and radius {a,b,c}
	float m_mass;
	glm::vec3 m_radii;

	// current state
	glm::vec3 m_c_position;
	glm::quat m_c_orientation;

	// rest state
	glm::vec3 m_r_position;
	glm::quat m_r_orientation;

	// predicted state
	glm::vec3 m_p_position;
	glm::quat m_p_orientation;

	// velocities
	glm::vec3 m_lin_velocity;
	glm::vec3 m_ang_velocity;
};