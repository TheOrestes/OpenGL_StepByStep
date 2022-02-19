
#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include <vector>

struct VertexP;
struct Texture;
class GLSLShader;
struct Material;

class Mesh
{
public:
	Mesh(std::vector<VertexP> vertices, std::vector<GLuint> indices);
	~Mesh();

	std::vector<VertexP>	m_vertices;
	std::vector<GLuint>		m_indices;

	void		Kill();

	void		SetShaderVariables(int shaderID, const glm::mat4& world);
	void		Render(GLSLShader* shader, const glm::mat4& world);

private:
	GLuint		vao;
	GLuint		vbo;
	GLuint		ibo;

	void		SetupMesh();
};