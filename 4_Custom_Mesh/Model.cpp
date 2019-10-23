
#include <iostream>
#include "Model.h"
#include "Mesh.h"
#include "VertexStructures.h"
#include "GLSLShader.h"
#include "TextureManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
Model::Model(const std::string& path)
{
	LoadModel(path);
}

//////////////////////////////////////////////////////////////////////////////////////////
Model::~Model()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void	Model::LoadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

	if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ASSIMP::ERROR = " << importer.GetErrorString() << std::endl; 
		return;
	}

	// retrieve directory path 
	m_Directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively...
	ProcessNode(scene->mRootNode, scene);
}

//////////////////////////////////////////////////////////////////////////////////////////
Mesh	Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<VertexP>	vertices;
	std::vector<GLuint>		indices;

	// loop through each vertex
	for (GLuint i = 0 ; i<mesh->mNumVertices ; i++)
	{
		VertexP vertex;
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertices.push_back(vertex);
	}

	// loop through each face to fetch indices
	for (GLuint j = 0 ; j<mesh->mNumFaces ; j++)
	{
		aiFace face = mesh->mFaces[j];

		// fetch all the indices
		for (GLuint k = 0 ; k < face.mNumIndices ; k++)
		{
			indices.push_back(face.mIndices[k]);	
		}
	}

	// return  a mesh object created from the extracted mesh data
	return Mesh(vertices, indices);
}

//////////////////////////////////////////////////////////////////////////////////////////
void	Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (GLuint i = 0 ; i<node->mNumMeshes ; i++)
	{
		// node contains only indices to actual objects in the scene. But, scene contains
		// all the data, node is just to keep things organized.
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.push_back(ProcessMesh(mesh, scene));
	}

	// once we have processed all the meshes, we recursively process each of the children nodes
	for (GLuint j = 0 ; j<node->mNumChildren ; j++)
	{
		ProcessNode(node->mChildren[j], scene);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void	Model::Render(GLSLShader* shader, const glm::mat4& world)
{
	for (GLuint i = 0 ; i<m_Meshes.size() ; i++)
	{
		m_Meshes[i].Render(shader, world);
	}
}
