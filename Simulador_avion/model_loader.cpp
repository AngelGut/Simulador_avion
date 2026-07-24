// ============================================================
// ARCHIVO: model_loader.cpp
// DESCRIPCION: Implementación del cargador de modelos 3D
// ============================================================

#include "model_loader.h"
#include <GL/glut.h>
#include <iostream>

Model::Model() : loaded(false) {}

Model::~Model() {}

bool Model::loadModel(const char* path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error al cargar modelo: " << importer.GetErrorString() << std::endl;
        return false;
    }

    std::cout << "Modelo cargado: " << path << std::endl;
    std::cout << "Meshes: " << scene->mNumMeshes << std::endl;

    processNode(scene->mRootNode, scene);

    loaded = true;
    return true;
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Procesar vértices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        if (mesh->HasNormals()) {
            vertex.normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        } else {
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Procesar índices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    Mesh newMesh;
    newMesh.vertices = vertices;
    newMesh.indices = indices;

    std::cout << "  Mesh: " << vertices.size() << " vértices, "
              << indices.size() / 3 << " triángulos" << std::endl;

    meshes.push_back(newMesh);
}

void Mesh::draw() {
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < indices.size(); i++) {
        unsigned int idx = indices[i];
        if (idx < vertices.size()) {
            const Vertex& v = vertices[idx];
            glNormal3f(v.normal.x, v.normal.y, v.normal.z);
            glVertex3f(v.position.x, v.position.y, v.position.z);
        }
    }
    glEnd();
}

void Model::draw() {
    if (!loaded) {
        std::cerr << "Modelo no cargado\n";
        return;
    }

    glColor3f(0.85f, 0.85f, 0.85f);
    for (auto& mesh : meshes) {
        mesh.draw();
    }
}
