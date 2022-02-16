
#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "../Gameobjects/GLLight.h"

#include <vector>

class BBoxCube;

class DirectionalLightObject
{
public:
	DirectionalLightObject();
	DirectionalLightObject(const glm::vec3& color);
	~DirectionalLightObject();

	void				Init();
	void				Kill();
	void				Update(float dt);
	void				UpdateBounds();
	void				Render();

	// Getter
	
	inline glm::vec3	GetLightAngleXYZ()	{ return m_vecLightAngleXYZ; }
	inline glm::vec3	GetLightDirection() { return m_vecLightDirection; }
	inline glm::vec3	GetLightColor()		{ return m_vecLightColor; }
	inline float		GetLightIntensity()	{ return m_fIntensity; }

	inline glm::mat4	GetWorldToLightViewMatrix()		 { return m_matWorldToLightViewMatrix; }
	inline glm::mat4	GetLightViewToProjectionMatrix() { return m_matLightViewToProjectionMatrix; }

	inline float		GetShadowBound()			{ return m_fShadowBounds; }
	inline float		GetShadowNearPlane()		{ return m_fShadowNearPlane; }
	inline float		GetShadowFarPlane()			{ return m_fShadowFarPlane; }
	inline bool			IsShowDebugShadowVolumeOn() { return m_bShowDebugShadowVolume; }

	// Setter
	void				SetLightAngleXYZ(const glm::vec3& angleXYZ);
	inline void			SetLightColor(const glm::vec3& col)			{ m_vecLightColor = col; };
	inline void			SetLightIntensity(float intensity)			{ m_fIntensity = intensity; }
	inline void			SetLightDirection(const glm::vec3& dir)		{ m_vecLightDirection = dir; }

	void				SetWorldToLightViewMatrix(const glm::mat4& _lightViewMatrix);
	void				SetLightViewToProjectionMatrix(const glm::mat4& _lightProjMatrix);

	void				SetShadowBound(float _bound)		 { m_fShadowBounds = _bound; }
	void				SetShadowNearPlane(float _nearPlane) { m_fShadowNearPlane = _nearPlane; }
	void				SetShadowFarPlane(float _farPlane)	 { m_fShadowFarPlane = _farPlane; }
	void				SetShowDebugShadowVolume(bool _show) { m_bShowDebugShadowVolume = _show; }

private:
	glm::vec3			m_vecLightAngleXYZ;
	glm::vec3			m_vecLightDirection;
	glm::vec3			m_vecLightColor;
	float				m_fIntensity;

	glm::mat4			m_matWorld;
	glm::mat4			m_matWorldToLightViewMatrix;
	glm::mat4			m_matLightViewToProjectionMatrix;

	BBoxCube*			m_pDebugShadowVolume;
	float				m_fShadowNearPlane;
	float				m_fShadowFarPlane;
	float				m_fShadowBounds;
	bool				m_bShowDebugShadowVolume;
};
