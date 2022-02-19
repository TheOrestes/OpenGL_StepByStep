
#include <sstream>
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "VertexStructures.h"
#include "GLSLShader.h"
#include "GLSkybox.h"
#include "PointLightObject.h"
#include "LightsManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
Mesh::Mesh(std::vector<VertexPTN> vertices, std::vector<GLuint> indices)
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
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VertexPTN), &m_vertices[0], GL_STATIC_DRAW);

	// index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);

	// vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)0);

	// texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, uv));

	// normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, normal));

	glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Mesh::SetMaterialProperties(int shaderID, Material * mat)
{
	glUniform4fv(glGetUniformLocation(shaderID, "material.Albedo"), 1, glm::value_ptr(mat->m_colAlbedo));
	glUniform4fv(glGetUniformLocation(shaderID, "material.Roughness"), 1, glm::value_ptr(mat->m_colRoughness));
}

//////////////////////////////////////////////////////////////////////////////////////////
void Mesh::PointLightIlluminance(int shaderID)
{
	int numPointLights = LightsManager::getInstance()->GetPointlightsCount();
	glUniform1i(glGetUniformLocation(shaderID, "numPointLights"), numPointLights);

	for (GLuint i = 0; i<numPointLights; ++i)
	{
		PointLightObject* light = LightsManager::getInstance()->GetPointLight(i);

		glm::vec3 position = light->GetLightPosition();
		glm::vec4 color = light->GetLightColor();
		float intensity = light->GetLightIntensity();
		float radius = light->GetLightRadius();

		// form a string out of point light Ids
		std::string pointLightPosStr = "pointLights[" + std::to_string(i) + "].position";
		std::string pointLightColStr = "pointLights[" + std::to_string(i) + "].color";
		std::string pointLightIntStr = "pointLights[" + std::to_string(i) + "].intensity";
		std::string pointLightRadStr = "pointLights[" + std::to_string(i) + "].radius";

		glUniform3fv(glGetUniformLocation(shaderID, pointLightPosStr.c_str()), 1, glm::value_ptr(position));
		glUniform4fv(glGetUniformLocation(shaderID, pointLightColStr.c_str()), 1, glm::value_ptr(color));
		glUniform1f(glGetUniformLocation(shaderID, pointLightIntStr.c_str()), intensity);
		glUniform1f(glGetUniformLocation(shaderID, pointLightRadStr.c_str()), radius);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Mesh::SetShaderVariables( int shaderID, const glm::mat4& world, Material* material)
{
	//--- Transformation matrices
	glm::mat4 projection = Camera::getInstance().getProjectionMatrix();
	glm::mat4 view = Camera::getInstance().getViewMatrix();
	glm::vec3 camPosition = Camera::getInstance().getCameraPosition();

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "matProj"), 1, GL_FALSE, glm::value_ptr(projection));	
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "matView"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "matWorld"), 1, GL_FALSE, glm::value_ptr(world));

	// calculate Inverse Transpose World matrix for transforming normals!
	glm::mat4 worldInvTranspose = glm::inverseTranspose(world);
	GLuint hWorldInvTranspose = glGetUniformLocation(shaderID, "matWorldInvTranspose");
	glUniformMatrix4fv(hWorldInvTranspose, 1, GL_FALSE, glm::value_ptr(worldInvTranspose));

	// set camera position uniform variable
	GLuint hCamPosition = glGetUniformLocation(shaderID, "cameraPosition");
	glUniform3fv(hCamPosition, 1, glm::value_ptr(camPosition));

	// Consider lighting
	PointLightIlluminance(shaderID);

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

	// Skybox reflection
	GLuint hCubeMap = glGetUniformLocation(shaderID, "texture_skybox");
	glUniform1i(hCubeMap, i + 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material->m_pTexAlbedo.getID());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, material->m_pTexSpecular.getID());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, material->m_pTexNormal.getID());

	// Bind skybox texture
	glActiveTexture(GL_TEXTURE0 + (i + 1));
	GLSkybox::getInstance().BindCubemap();

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

	// unbind skybox texture
	glActiveTexture(GL_TEXTURE0 + (i + 1));
	GLSkybox::getInstance().UnbindCubemap();
}

//////////////////////////////////////////////////////////////////////////////////////////


