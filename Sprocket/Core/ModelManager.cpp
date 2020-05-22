#include "ModelManager.h"
#include "Log.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Sprocket {
namespace {

bool isSceneValid(const aiScene* scene)
    // Returns true if the scene is valid and false otherwise.
{
    return scene && 
           !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) &&
           scene->mRootNode;
}

Model3D processMesh(const aiScene* scene, aiMesh* mesh)
{    
    Vertex3DBuffer vertices;
    IndexBuffer    indices;

    // Vertices
    for (unsigned int i = 0; i != mesh->mNumVertices; ++i) {
        Vertex3D vertex;

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        // TODO: Add error checking code here to make sure the texture exists
        vertex.textureCoords.x = mesh->mTextureCoords[0][i].x;
        vertex.textureCoords.y = mesh->mTextureCoords[0][i].y;

        vertices.push_back(vertex);
    }

    // Indices
    for (unsigned int i = 0; i != mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j != face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    return Model3D(vertices, indices);
}

}

Model3D ModelManager::loadModel(const std::string& path)
{
    Assimp::Importer importer;
    int flags = aiProcess_Triangulate | aiProcess_FlipUVs;
    const aiScene* scene = importer.ReadFile(path, flags);

    if (!isSceneValid(scene)) {
        SPKT_LOG_ERROR("ERROR::ASSIMP::{}", importer.GetErrorString());
        return Model3D();
    }

    if (scene->mNumMeshes != 1) {
        SPKT_LOG_ERROR("File does not have exactly one mesh, not supported!");
        return Model3D();
    }

    return processMesh(scene, scene->mMeshes[0]);
}

Model3D ModelManager::loadModel(const std::string& name,
                                const std::string& path)
{
    auto it = d_loadedModels.find(name);
    if (it != d_loadedModels.end()) {
        SPKT_LOG_ERROR("Tried to cache a model as '{}', which exists!", name);
        return it->second;
    }
    Model3D model = loadModel(path);
    d_loadedModels.insert(std::make_pair(name, model));
    return model;   
}

Model3D ModelManager::getModel(const std::string& name) const
{
    auto it = d_loadedModels.find(name);
    if (it == d_loadedModels.end()) {
        SPKT_LOG_ERROR("Tried to load model '{}', which does not exist!", name);
        return Model3D();
    }
    return it->second;
}

ModelManager::Map::iterator ModelManager::begin()
{
    return d_loadedModels.begin();
}

ModelManager::Map::iterator ModelManager::end()
{
    return d_loadedModels.end();
}

ModelManager::Map::const_iterator ModelManager::cbegin() const
{
    return d_loadedModels.cbegin();
}

ModelManager::Map::const_iterator ModelManager::cend() const
{
    return d_loadedModels.cend();
}


}