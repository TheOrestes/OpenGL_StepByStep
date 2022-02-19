
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
	
	void					SetPosition(const glm::vec3& pos);
	void					SetRotation(const glm::vec3& axis, float angle);
	void					SetScale(const glm::vec3& sc);
	
	inline void				SetPath(const std::string& path){ m_strPath = path; }
	inline void				SetShader(const std::string& shader){ m_strShader = shader; }

private:
	std::string				m_strPath;
	std::string				m_strShader;

	GLSLShader*				m_pShader;
	GLSLShader*				m_pShadowPassShader;
	Model*					m_pModel;
	Material*				m_pMaterial;

	glm::vec3				m_vecPosition;
	glm::vec3				m_vecRotation;
	float					m_fAngle;
	glm::vec3				m_vecScale;
	glm::mat4				m_matWorld;
};

