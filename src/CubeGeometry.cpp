#include "pch.h"
#include "CommonGeometry.h"
#include "CubeGeometry.h"

CubeGeometry::CubeGeometry()
{
    glGenVertexArrays(1, &m_cube_vao);
    glGenBuffers(1, &m_cube_vbo);

    glBindVertexArray(m_cube_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CommonGeometry::cube_vertices), &CommonGeometry::cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

CubeGeometry::~CubeGeometry()
{
    glDeleteVertexArrays(1, &m_cube_vao);
    glDeleteBuffers(1, &m_cube_vbo);
}

void CubeGeometry::Draw()
{
    glBindVertexArray(m_cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

CubeGeometry& CubeGeometry::GetCubeGeometry()
{
    static CubeGeometry cube_geom;
    return cube_geom;
}