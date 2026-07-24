// ============================================================
// ARCHIVO: model_loader.cpp
// DESCRIPCION: Implementación del cargador de modelos 3D
// ============================================================

#include "model_loader.h"
#include <GL/glut.h>
#include <iostream>
#include <algorithm>
#include <cfloat>

Model::Model() : loaded(false), scale(1.0f), center(0.0f) {}

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
    normalizeModel();

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
        }
        else {
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

void Model::normalizeModel() {
    if (meshes.empty()) return;

    glm::vec3 minBounds(FLT_MAX);
    glm::vec3 maxBounds(-FLT_MAX);
    int vertexCount = 0;

    for (auto& mesh : meshes) {
        for (auto& vertex : mesh.vertices) {
            minBounds.x = std::min(minBounds.x, vertex.position.x);
            minBounds.y = std::min(minBounds.y, vertex.position.y);
            minBounds.z = std::min(minBounds.z, vertex.position.z);

            maxBounds.x = std::max(maxBounds.x, vertex.position.x);
            maxBounds.y = std::max(maxBounds.y, vertex.position.y);
            maxBounds.z = std::max(maxBounds.z, vertex.position.z);
            vertexCount++;
        }
    }

    glm::vec3 modelSize = maxBounds - minBounds;
    float maxDim = std::max({modelSize.x, modelSize.y, modelSize.z});

    // Escalar a 1.5f (dejando margen para rotaciones)
    scale = 1.5f / maxDim;
    center = (minBounds + maxBounds) * 0.5f;

    std::cout << "  Total vértices: " << vertexCount << std::endl;
    std::cout << "  Size: [" << modelSize.x << ", " << modelSize.y << ", " << modelSize.z << "]" << std::endl;
    std::cout << "  Scale: " << scale << ", Center: [" << center.x << ", " << center.y << ", " << center.z << "]" << std::endl;

    for (auto& mesh : meshes) {
        for (auto& vertex : mesh.vertices) {
            vertex.position = (vertex.position - center) * scale;
        }
    }
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