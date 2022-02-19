
#include <sstream>
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "VertexStructures.h"
#include "GLSLShader.h"
#include "GLSkybox.h"

//////////////////////////////////////////////////////////////////////////////////////////
Mesh::Mesh(std::vector<VertexPT> vertices, std::vector<GLuint> indices)
{
	m_vertices = vertices;
	m_indices = indices;

	SetupMesh();
}

//////////////////////////////////////////////////////////////////////////////////////////
Mesh::~Mesh()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
void	Mesh::SetupMesh()
{
	// vertex array buffer
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VertexPT), &m_vertices[0], GL_STATIC_DRAW);

	// index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);

	// vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (void*)0);

	// texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (void*)offsetof(VertexPT, uv));

	glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Mesh::SetMaterialProperties(int shaderID, Material * mat)
{
	glUniform4fv(glGetUniformLocation(shaderID, "material.Albedo"), 1, glm::value_ptr(mat->m_colAlbedo));
	glUniform4fv(glGetUniformLocation(shaderID, "material.Roughness"), 1, glm::value_ptr(mat->m_colRoughness));
}

//////////////////////////////////////////////////////////////////////////////////////////
void Mesh::SetShaderVariables( int shaderID, const glm::mat4& world, Material* material)
{
	//--- Transformation matrices
	glm::mat4 projection = Camera::getInstance().getProjectionMatrix();
	glm::mat4 view = Camera::getInstance().getViewMatrix();

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "matProj"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "matView"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "matWorld"), 1, GL_FALSE, glm::value_ptr(world));

	// Set Material properties
	SetMaterialProperties(shaderID, material);
}	

//////////////////////////////////////////////////////////////////////////////////////////
void	Mesh::Render(GLSLShader* shader, const glm::mat4& world, Material* material)
{
	GLuint diffuseNr = 1;

	GLuint shaderID = shader->GetShaderID();
	shader->Use();

	GLuint i = 0;
	if (material->m_pTexAlbedo.getHasTexture())
	{
		glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse"), 0);
	}

	if (material->m_pTexSpecular.getHasTexture())
	{
		++i;
		glUniform1i(glGetUniformLocation(shaderID, "texture_specular"), 1);
	}

	if (material->m_pTexNormal.getHasTexture())
	{
		++i;
		glUniform1i(glGetUniformLocation(shaderID, "texture_normal"), 2);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material->m_pTexAlbedo.getID());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, material->m_pTexSpecular.getID());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, material->m_pTexNormal.getID());

	// Draw mesh
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
	glEnable(GL_DEPTH_TEST);

	// Set all Shader variables...
	SetShaderVariables(shaderID, world, material);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////


