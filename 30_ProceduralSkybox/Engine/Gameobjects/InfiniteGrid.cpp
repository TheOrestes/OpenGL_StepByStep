#include "InfiniteGrid.h"
#include "Camera.h"
#include "../Rendering/GLSLShader.h"
#include "../Rendering/ScreenAlignedQuad.h"
#include "../Main/Helper.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
InfiniteGrid::InfiniteGrid()
{
	m_bEnabled = true;
	m_fCellSize = 1.0f;
	m_fFadeDistance = 500.0f;

	m_pShader = nullptr;
	m_pScreenQuad = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
InfiniteGrid::~InfiniteGrid()
{
	SAFE_DELETE(m_pShader);
	SAFE_DELETE(m_pScreenQuad);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void InfiniteGrid::Initialize()
{
	m_pShader = new GLSLShader("Shaders/InfiniteGrid.vert", "Shaders/InfiniteGrid.frag");

	m_pScreenQuad = new ScreenAlignedQuad();
	m_pScreenQuad->CreateScreenAlignedQuad(QuadDesc::MAIN);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void InfiniteGrid::Render(GLuint positionBuffer, GLuint objectIDBuffer)
{
	if (!m_bEnabled)
		return;

	m_pShader->Use();
	GLuint shaderID = m_pShader->GetShaderID();

	glm::mat4 matView = Camera::getInstance().getViewMatrix();
	glm::mat4 matProj = Camera::getInstance().getProjectionMatrix();
	glm::mat4 matInvViewProj = glm::inverse(matProj * matView);
	glm::vec3 cameraPosition = Camera::getInstance().getCameraPosition();

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "matInvViewProj"), 1, GL_FALSE, glm::value_ptr(matInvViewProj));
	glUniform3fv(glGetUniformLocation(shaderID, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
	glUniform1f(glGetUniformLocation(shaderID, "cellSize"), m_fCellSize);
	glUniform1f(glGetUniformLocation(shaderID, "fadeDistance"), m_fFadeDistance);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, positionBuffer);
	glUniform1i(glGetUniformLocation(shaderID, "positionBuffer"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, objectIDBuffer);
	glUniform1i(glGetUniformLocation(shaderID, "objectIDBuffer"), 1);

	// Unlit overlay blended on top of the already-lit scene color - see InfiniteGrid.frag for why
	// this can't be resolved as an opaque G-buffer surface (partial line coverage needs blending).
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pScreenQuad->RenderToScreenAlignedQuad();

	glDisable(GL_BLEND);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
}
