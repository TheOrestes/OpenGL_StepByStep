
#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>

struct VertexPTN;
struct Texture;
class GLSLShader;
struct Material;

class Mesh
{
public:
	Mesh(std::vector<VertexPTN> vertices, std::vector<GLuint> indices);
	~Mesh();

	std::vector<VertexPTN>	m_vertices;
	std::vector<GLuint>		m_indices;

	void		Kill();

	void		SetMaterialProperties(int shaderID, Material* mat);
	void		PointLightIlluminance(int shaderID);
	void		SetShaderVariables(int shaderID, const glm::mat4& world, Material* mat);
	void		Render(GLSLShader* shader, const glm::mat4& world, Material* mat);

private:
	GLuint		vao;
	GLuint		vbo;
	GLuint		ibo;

	void		SetupMesh();
};