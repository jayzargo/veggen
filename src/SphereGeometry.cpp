#include "pch.h"
#include "CommonGeometry.h"
#include "SphereGeometry.h"

SphereGeometry::SphereGeometry(int x_samples, int y_samples)
{
	std::vector<CommonGeometry::Vertex> vertices;
	std::vector<unsigned int> indices;

	size_t reserved_vertices_mem = static_cast<size_t>(x_samples + 1) * static_cast<size_t>(y_samples + 1);
	// quad -> 2 triangles, 6 vertices per quad
	size_t reserved_indices_mem = static_cast<size_t>(x_samples) * static_cast<size_t>(y_samples) * 6;

	vertices.reserve(reserved_vertices_mem);
	indices.reserve(reserved_indices_mem);

	// generating sphere vertices
	for (int x_sample = 0; x_sample <= x_samples; x_sample++) {
		float u_coord = static_cast<float>(x_sample) / static_cast<float>(x_samples);
		// get spherical coordinates from u,v - phi angle (0,pi)
		float phi = glm::pi<float>() * u_coord;
		float y = cos(phi);
		
		for (int y_sample = 0; y_sample <= y_samples; y_sample++)
		{
			float v_coord = static_cast<float>(y_sample) / static_cast<float>(y_samples);
			// get spherical coordinates from u,v - theta angle (0,2pi)
			float theta = 2.0f * glm::pi<float>() * v_coord;

			float x = sin(phi) * cos(theta);
			float z = sin(phi) * sin(theta);


			vertices.push_back(
				CommonGeometry::Vertex{
					glm::vec3(x,y,z),
					glm::vec3(x,y,z),
					glm::vec2(u_coord, v_coord)
				}
			);
		}
	}

	// generate sphere indices
	for (int x_sample = 0; x_sample < x_samples; x_sample++) {
		for (int y_sample = 0; y_sample < y_samples; y_sample++) {
			int current = x_sample * (y_samples + 1) + y_sample;
			int next = current + y_samples + 1;

			indices.push_back(current);
			indices.push_back(next);
			indices.push_back(current + 1);

			indices.push_back(current + 1);
			indices.push_back(next);
			indices.push_back(next + 1);
		}
	}

	m_indices_cnt = indices.size();

	glGenVertexArrays(1, &m_sphere_vao);
	glGenBuffers(1, &m_sphere_vbo);
	glGenBuffers(1, &m_sphere_ebo);

	glBindVertexArray(m_sphere_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
	glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(CommonGeometry::Vertex),vertices.data(),GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphere_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CommonGeometry::Vertex), (void*)offsetof(CommonGeometry::Vertex, position));
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CommonGeometry::Vertex), (void*)offsetof(CommonGeometry::Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CommonGeometry::Vertex), (void*)offsetof(CommonGeometry::Vertex, uv));

	glBindVertexArray(0);
}

SphereGeometry::~SphereGeometry()
{
	std::cout << "sphere geometry deleted" << "\n";
	glDeleteVertexArrays(1, &m_sphere_vao);
	glDeleteBuffers(1, &m_sphere_ebo);
	glDeleteBuffers(1, &m_sphere_vbo);
}

SphereGeometry& SphereGeometry::GetSphereGeometry(int x_samples, int y_samples)
{
	static SphereGeometry sphere_geom(x_samples, y_samples);
	return sphere_geom;
}

void SphereGeometry::Draw() 
{
	glBindVertexArray(m_sphere_vao);
	glDrawElements(GL_TRIANGLES, m_indices_cnt, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}