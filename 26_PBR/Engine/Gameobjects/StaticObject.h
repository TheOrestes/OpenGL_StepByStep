
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
	inline std::string		GetName() { return m_strName; }
	inline std::string		GetPath() { return m_strPath; }
	inline std::string		GetShaderName() { return m_strShader; }
	inline glm::vec3		GetPosition() { return m_vecPosition; }
	inline glm::vec3		GetRotationAxis() { return m_vecRotation; }
	inline glm::vec3		GetScale() { return m_vecScale; }
	inline Material*		GetMaterialPtr() { return m_pMaterial; }
	inline float			GetCurrentAngle() { return m_fAngle; }
	inline bool				GetAutoRotateFlag() { return m_bAutoRotate; }
	inline float			GetAutoRotateSpeed() { return m_fAutoRotateSpeed; }
	

	inline void				SetName(const std::string& name) { m_strName = name; }
	inline void				SetPath(const std::string& path){ m_strPath = path; }
	inline void				SetShader(const std::string& shader){ m_strShader = shader; }
	inline void				SetPosition(const glm::vec3& pos) { m_vecPosition = pos; }
	inline void				SetRotationAxis(const glm::vec3& rotAxis) { m_vecRotation = rotAxis; }
	inline void				SetRotationAngle(float angle) { m_fCurrentAngle = angle; }
	inline void				SetScale(const glm::vec3& scale) { m_vecScale = scale; }
	inline void				SetAutoRotate(bool _flag) { m_bAutoRotate = _flag; }
	inline void				SetAutoRotateSpeed(float _speed) { m_fAutoRotateSpeed = _speed; }

private:
	std::string				m_strName;
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

	bool					m_bAutoRotate;
	float					m_fAutoRotateSpeed;
	float					m_fCurrentAngle;
};

