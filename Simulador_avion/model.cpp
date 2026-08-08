#include "model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

namespace {
    void processMesh(aiMesh* mesh, const aiMatrix4x4& transform, MeshData& outMesh) {
        outMesh.vertices.reserve(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            aiVector3D pos = transform * mesh->mVertices[i];

            aiVector3D normal(0.0f, 1.0f, 0.0f);
            if (mesh->HasNormals()) {
                aiMatrix3x3 normalMatrix(transform); // rotación/escala, sin traslación
                normal = normalMatrix * mesh->mNormals[i];
                normal.Normalize();
            }

            Vertex v;
            v.position = glm::vec3(pos.x, pos.y, pos.z);
            v.normal = glm::vec3(normal.x, normal.y, normal.z);
            outMesh.vertices.push_back(v);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                outMesh.indices.push_back(face.mIndices[j]);
        }
    }

    namespace {
        bool shouldSkipMesh(const std::string& name) {
            return name == "Plane_0"; 
        }
    }

    void processNode(aiNode* node, const aiScene* scene, const aiMatrix4x4& parentTransform, Model& outModel) {
        aiMatrix4x4 nodeTransform = parentTransform * node->mTransformation;

        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            if (shouldSkipMesh(mesh->mName.C_Str())) continue;

            MeshData meshData;
            processMesh(mesh, nodeTransform, meshData);
            outModel.meshes.push_back(std::move(meshData));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene, nodeTransform, outModel);
    }
}



bool loadModel(const std::string& path, Model& outModel) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        std::cout << "Error de Assimp cargando '" << path << "': " << importer.GetErrorString() << "\n";
        return false;
    }

    aiMatrix4x4 identity;
    processNode(scene->mRootNode, scene, identity, outModel);
    outModel.loaded = true;
    return true;
}