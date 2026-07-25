// ============================================================
// ARCHIVO: model_loader.cpp
// DESCRIPCION: Implementación del cargador de modelos 3D
// ============================================================

#include "model_loader.h"
#include <GL/glut.h>
#include <iostream>
#include <algorithm>
#include <cfloat>
#include <glm/gtc/matrix_transform.hpp>

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

    glm::mat4 identity(1.0f);
    processNode(scene->mRootNode, scene, identity);
    normalizeModel();

    loaded = true;
    return true;
}

void Model::processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform) {
    // Convertir transformación de Assimp a GLM
    glm::mat4 nodeTransform(1.0f);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            nodeTransform[j][i] = node->mTransformation[i][j];
        }
    }

    glm::mat4 currentTransform = parentTransform * nodeTransform;

    // Procesar meshes del nodo
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, currentTransform);
    }

    // Procesar hijos recursivamente
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, currentTransform);
    }
}

void Model::processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& nodeTransform) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Procesar vértices con transformación de nodo
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Aplicar transformación del nodo a la posición
        glm::vec4 pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
        glm::vec4 transformedPos = nodeTransform * pos;
        vertex.position = glm::vec3(transformedPos) / transformedPos.w;

        // Aplicar transformación a normales (sin traslación)
        if (mesh->HasNormals()) {
            glm::vec4 norm(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 0.0f);
            glm::vec4 transformedNorm = nodeTransform * norm;
            vertex.normal = glm::normalize(glm::vec3(transformedNorm));
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

float Model::getRecommendedZoom() const {
    if (!loaded || scale <= 0.0f) return -4.0f;

    // Si el modelo es muy pequeño (scale muy grande), acercamos más
    // Si el modelo es muy grande (scale muy pequeño), alejamos más
    float inverseScale = 1.0f / scale;

    // Zoom adaptativo: entre -2.5 (muy cerca) y -8.0 (muy lejos)
    float zoomValue = -4.0f - (inverseScale * 0.3f);
    zoomValue = std::max(zoomValue, -10.0f);  // No más lejos que -10
    zoomValue = std::min(zoomValue, -1.5f);   // No más cerca que -1.5

    std::cout << "  Scale: " << scale << " → Zoom: " << zoomValue << std::endl;

    return zoomValue;
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