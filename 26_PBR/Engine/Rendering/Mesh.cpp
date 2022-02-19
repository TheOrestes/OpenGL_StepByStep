
#include <sstream>
#include "Mesh.h"
#include "VertexStructures.h"
#include "GLSLShader.h"

#include "../Gameobjects/Material.h"
#include "../Gameobjects/Camera.h"
#include "../Gameobjects/GLSkybox.h"
#include "../Lighting/PointLightObject.h"
#include "../Lighting/DirectionalLightObject.h"
#include "../Lighting/LightsManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
Mesh::Mesh(std::vector<VertexPTNBT> vertices, std::vector<GLuint> indices)
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
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VertexPTNBT), &m_vertices[0], GL_STATIC_DRAW);

	// index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);

	// vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTNBT), (void*)0);

	// texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTNBT), (void*)offsetof(VertexPTNBT, uv));

	// normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTNBT), (void*)offsetof(VertexPTNBT, normal));

	// binormals
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTNBT), (void*)offsetof(VertexPTNBT, binormal));

	// tangents
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTNBT), (void*)offsetof(VertexPTNBT, tangent));

	// barycentric
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTNBT), (void*)offsetof(VertexPTNBT, barycentric));

	glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Mesh::SetMaterialProperties(int shaderID, Material * mat)
{
	glUniform4fv(glGetUniformLocation(shaderID, "material.Albedo"), 1, glm::value_ptr(mat->m_colAlbedo));
	glUniform4fv(glGetUniformLocation(shaderID, "material.Emission"), 1, glm::value_ptr(mat->m_colEmission));
	glUniform1fv(glGetUniformLocation(shaderID, "material.Roughness"), 1,  &(mat->m_fRoughness));
	glUniform1fv(glGetUniformLocation(shaderID, "material.Metallic"), 1, &(mat->m_fMetallic));
	glUniform1fv(glGetUniformLocation(shaderID, "material.Occlusion"), 1, &(mat->m_fOcclusion));
	glUniform1fv(glGetUniformLocation(shaderID, "material.Height"), 1, &(mat->m_fHeight));

	// toggle wireframe on/off...!
	if (mat->m_bWireframe)
	{
		glUniform1f(glGetUniformLocation(shaderID, "wireframeWidth"), 0.75f);
	}
	else
	{
		glUniform1f(glGetUniformLocation(shaderID, "wireframeWidth"), 0.0f);
	}
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

	// calculate Inverse Transpose World matrix for transforming normals!
	glm::mat4 worldInvTranspose = glm::inverseTranspose(world);
	GLuint hWorldInvTranspose = glGetUniformLocation(shaderID, "matWorldInvTranspose");
	glUniformMatrix4fv(hWorldInvTranspose, 1, GL_FALSE, glm::value_ptr(worldInvTranspose));

	// Set Material properties
	SetMaterialProperties(shaderID, material);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Mesh::SetShadowPassMaterialShaderVariables(int shaderID, const glm::mat4& world)
{
	DirectionalLightObject* mainSunLight = LightsManager::getInstance()->GetDirectionalLight(0);
	
	GLuint hWorld = glGetUniformLocation(shaderID, "matWorld");
	GLuint hWorldToLightView = glGetUniformLocation(shaderID, "matWorldToLightView");
	GLuint hLightViewToProjection = glGetUniformLocation(shaderID, "matLightViewToProjection");

	glUniformMatrix4fv(hWorld, 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(hWorldToLightView, 1, GL_FALSE, glm::value_ptr(mainSunLight->GetWorldToLightViewMatrix()));
	glUniformMatrix4fv(hLightViewToProjection, 1, GL_FALSE, glm::value_ptr(mainSunLight->GetLightViewToProjectionMatrix()));
}

//////////////////////////////////////////////////////////////////////////////////////////
void	Mesh::Render(GLSLShader* shader, const glm::mat4& world, Material* material)
{
	GLuint diffuseNr = 1;

	GLuint shaderID = shader->GetShaderID();
	shader->Use();

	if (material->m_pTexAlbedo.getHasTexture())
	{
		glUniform1i(glGetUniformLocation(shaderID, "hasDiffuse"), true);
		glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse"), 0);
	}

	if (material->m_pTexMask.getHasTexture())
	{
		glUniform1i(glGetUniformLocation(shaderID, "hasMask"), true);
		glUniform1i(glGetUniformLocation(shaderID, "texture_mask"), 1);
	}

	if (material->m_pTexNormal.getHasTexture())
	{
		glUniform1i(glGetUniformLocation(shaderID, "hasNormal"), true);
		glUniform1i(glGetUniformLocation(shaderID, "texture_normal"), 2);
	}

	if (material->m_pTexEmission.getHasTexture())
	{
		glUniform1i(glGetUniformLocation(shaderID, "hasEmissive"), true);
		glUniform1i(glGetUniformLocation(shaderID, "texture_emissive"), 3);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material->m_pTexAlbedo.getID());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, material->m_pTexMask.getID());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, material->m_pTexNormal.getID());


	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, material->m_pTexEmission.getID());

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

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void	Mesh::RenderShadowMap(GLSLShader* shader, const glm::mat4& world)
{
	GLuint shaderID = shader->GetShaderID();
	shader->Use();

	// Draw mesh
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
	glEnable(GL_DEPTH_TEST);

	// Set all Shader variables...
	SetShadowPassMaterialShaderVariables(shaderID, world);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


