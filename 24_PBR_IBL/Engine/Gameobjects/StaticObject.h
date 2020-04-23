
#pragma once

#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Model;
class GLSLShader;
struct	StaticObjectData;
struct Material;

class StaticObject
{
public:
	StaticObject();
	StaticObject(const StaticObjectData& data);
	virtual ~StaticObject();

	void					Init();
	void					Kill();
	void					Update(float dt);
	void					Render();
	void					RenderShadowMap();

	//---- GETTER - SETTER!
	
	inline Material*		GetMaterialPtr() { return m_pMaterial; }

public:
	std::string				m_strName;
	std::string				m_strPath;
	std::string				m_strShaderName;

	glm::vec3				m_vecPosition;
	glm::vec3				m_vecRotationAxis;
	float					m_fAngle;
	glm::vec3				m_vecScale;
	glm::mat4				m_matWorld;

	bool					m_bAutoRotate;
	float					m_fAutoRotateSpeed;
	float					m_fCurrentRotationAngle;

private:
	GLSLShader*				m_pShader;
	GLSLShader*				m_pShadowPassShader;
	Model*					m_pModel;
	Material*				m_pMaterial;
};

