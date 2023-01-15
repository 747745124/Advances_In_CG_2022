#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "Shader.h"
#include "FileSystem.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <filesystem>

unsigned int PreviewImageFromFile(const std::string &path);
unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model
{
public:
	static std::unordered_map<std::string, Model *> modelList;		// ��������ģ�ͣ�������ʹ��
	static std::unordered_map<std::string, Model *> basicVoxelList; // �������л�������ģ��
public:
	enum ModelType
	{
		ComplexModel,
		BasicVoxel
	};
	// model data
	vector<Texture> textures_loaded; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;
	Texture *previewImage;

	// constructor, expects a filepath to a 3D model.
	Model(string const &path, ModelType type = ModelType::ComplexModel, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(FileSystem::getPath(path));
		if (type == ModelType::ComplexModel)
			modelList[path] = this;
		else if (type == ModelType::BasicVoxel)
			basicVoxelList[path] = this;
	}

	// draws the model, and thus all its meshes
	void Draw(Shader *shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	// load models from a path, which contains several folder of models.
	static int loadModelsFromPath(const string &modelsPath, ModelType type)
	{
		using namespace filesystem;
		int totalModels = 0;
		path basePath(modelsPath);
		cout << "Loading models from path: " << modelsPath << endl;
		if (!exists(basePath))
		{
			cout << "Error: Failed to load models from path: " << modelsPath;
			cout << ", File path doesn't exist!" << endl;
			return 0;
		}
		else if (directory_entry(basePath).status().type() != file_type::directory)
		{
			cout << "Error: \"" << modelsPath << "\" is not a directory." << endl;
			return 0;
		}
		totalModels = loadModelsFromPath_r(modelsPath, type);
		cout << totalModels << " models loaded." << endl;
		cout << "==============================================" << endl;
		return totalModels;
	}

private:
	// This function should only be called by function loadModelsFromPath.
	//  Used to "RECURSIVELY" load all objs in the folder.
	//  @retval: number of objects loaded.
	static int loadModelsFromPath_r(filesystem::path relPath, ModelType type)
	{
		using namespace filesystem;
		directory_iterator fileList1(relPath), fileList2(relPath);
		int totalModels = 0;
		cout << "Looking into " << relPath << " ..." << endl;

		// First pass, check if there is an obj file.
		string previewFilename = "", modelFilename = "";
		for (auto &it : fileList1)
		{
			path curPath = it.path();
			if (directory_entry(curPath).status().type() != file_type::regular)
				continue;
			if (curPath.extension() == ".obj" && curPath.filename() != "Bridge-2.obj" && curPath.filename() != "Building-1.obj" && curPath.filename() != "Light-2.obj" && curPath.filename() != "Light-3.obj")
				modelFilename = curPath.string();
			else if (curPath.filename() == "preview.png")
				previewFilename = curPath.string();
		}
		if (modelFilename != "")
		{
			cout << "Model " + modelFilename + " found." << endl;
			int relPathPos = modelFilename.find("model");
			Model *tmpModel = new Model(modelFilename.substr(relPathPos), type);
			if (previewFilename != "")
			{
				tmpModel->previewImage = new Texture;
				tmpModel->previewImage->id = PreviewImageFromFile(previewFilename);
				// cout<<"Preview img loaded:"<<tmpModel->previewImage->id<<endl;
			}
			else
			{
				cout << "No preview image found." << endl;
			}
			totalModels++;
		}

		// Second pass, look into child directories.
		for (auto &it : fileList2)
		{
			if (directory_entry(it.path()).status().type() != file_type::directory)
				continue;
			totalModels += loadModelsFromPath_r(it.path(), type);
		}
		return totalModels;
	}

	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const &path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene)
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene.
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		// data to fill
		vector<Vertex_Simple> vertices_simple;
		vector<Vertex_Extra> vertices_extra;
		vector<unsigned int> indices;
		vector<Texture> textures;

		// walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex_Simple vertex_simple;
			Vertex_Extra vertex_extra;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex_simple.Position = vector;
			// normals
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex_simple.Normal = vector;
			}
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex_simple.TexCoords = vec;
				// Tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex_extra.Tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex_extra.Bitangent = vector;
				vertices_extra.push_back(vertex_extra);
			}
			else
				vertex_simple.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices_simple.push_back(vertex_simple);
		}

		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		// return a mesh object created from the extracted mesh data
		return Mesh(vertices_simple, vertices_extra, indices, textures);
	}

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{ // if texture hasn't been loaded already, load it
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
};

#endif