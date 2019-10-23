
#include <iostream>
#include "Model.h"
#include "Mesh.h"
#include "VertexStructures.h"
#include "GLSLShader.h"
#include "TextureManager.h"
#include "Material.h"

//////////////////////////////////////////////////////////////////////////////////////////
Model::Model(const std::string& path, Material* mat)
{
	LoadModel(path, mat);
}

//////////////////////////////////////////////////////////////////////////////////////////
Model::~Model()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void	Model::LoadModel(const std::string& path, Material* material)
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
	ProcessNode(scene->mRootNode, scene, material);
}

//////////////////////////////////////////////////////////////////////////////////////////
Mesh	Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, Material* material)
{
	std::vector<VertexPT>	vertices;
	std::vector<GLuint>		indices;
	std::vector<Texture>	textures;

	// loop through each vertex
	for (GLuint i = 0 ; i<mesh->mNumVertices ; i++)
	{
		VertexPT vertex;
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		// Consider texture co-ordinate data!
		if (mesh->mTextureCoords[0])
		{
			vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.uv = glm::vec2(0, 0);
		}

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

	// Process Materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		aiString str;

		// Look for Diffuse Texture
		unsigned int nDiff = mat->GetTextureCount(aiTextureType_DIFFUSE);
		if (nDiff > 0)
		{
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
			const int id1 = TextureManager::getInstannce().Load2DTextureFromFile(str.C_Str(), m_Directory);
			material->m_pTexAlbedo.setID(id1);
			material->m_pTexAlbedo.SetHasTexture(true);
			material->m_pTexAlbedo.setType(TextureType::TEXTURE_ALBEDO);
			material->m_pTexAlbedo.setName("texture_diffuse");
			material->m_pTexAlbedo.setPath(str.C_Str());
		}

		// Look for Specular Texture
		unsigned int nSpecular = mat->GetTextureCount(aiTextureType_SPECULAR);
		if (nSpecular > 0)
		{
			mat->GetTexture(aiTextureType_SPECULAR, 0, &str);
			const int id2 = TextureManager::getInstannce().Load2DTextureFromFile(str.C_Str(), m_Directory);
			material->m_pTexSpecular.setID(id2);
			material->m_pTexSpecular.SetHasTexture(true);
			material->m_pTexSpecular.setType(TextureType::TEXTURE_SPECULAR);
			material->m_pTexSpecular.setName("texture_specular");
			material->m_pTexSpecular.setPath(str.C_Str());
		}

		// Look for Normal texture...
		unsigned int nNormal = mat->GetTextureCount(aiTextureType_NORMALS);
		if (nNormal > 0)
		{
			mat->GetTexture(aiTextureType_NORMALS, 0, &str);
			const int id3 = TextureManager::getInstannce().Load2DTextureFromFile(str.C_Str(), m_Directory);
			material->m_pTexNormal.setID(id3);
			material->m_pTexNormal.SetHasTexture(true);
			material->m_pTexNormal.setType(TextureType::TEXTURE_NORMAL);
			material->m_pTexNormal.setName("texture_normal");
			material->m_pTexNormal.setPath(str.C_Str());
		}
	}

	// return  a mesh object created from the extracted mesh data
	return Mesh(vertices, indices);
}

//////////////////////////////////////////////////////////////////////////////////////////
void	Model::ProcessNode(aiNode* node, const aiScene* scene, Material* material)
{
	for (GLuint i = 0 ; i<node->mNumMeshes ; i++)
	{
		// node contains only indices to actual objects in the scene. But, scene contains
		// all the data, node is just to keep things organized.
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.push_back(ProcessMesh(mesh, scene, material));
	}

	// once we have processed all the meshes, we recursively process each of the children nodes
	for (GLuint j = 0 ; j<node->mNumChildren ; j++)
	{
		ProcessNode(node->mChildren[j], scene, material);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void	Model::Render(GLSLShader* shader, const glm::mat4& world, Material* material)
{
	for (GLuint i = 0 ; i<m_Meshes.size() ; i++)
	{
		m_Meshes[i].Render(shader, world, material);
	}
}
