
#pragma once

#include <string>
#include <vector>
#include "VertexStructures.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

class Mesh;
class GLSLShader;
struct Material;

class Model
{
public:
	Model(const std::string& path, Material* mat);
	~Model();
	
	void					Render(GLSLShader* shader, const glm::mat4& world, Material* mat);

	// Required for Bounding Box!
	std::vector<glm::vec3>  GetVertexPositions();

private:
	std::vector<Mesh>		m_Meshes;
	std::string				m_Directory;

	void					LoadModel(const std::string& path, Material* material);
	void					ProcessNode(aiNode* node, const aiScene* scene, Material* material);
	Mesh					ProcessMesh(aiMesh* mesh, const aiScene* scene, Material* material);
};