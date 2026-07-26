// ============================================================
// ARCHIVO: model_loader.cpp
// DESCRIPCION: Implementación del cargador con VAO/VBO
// ============================================================

#include "model_loader.h"
#include "shader.h"
#include "stb_image_loader.h"
#include <GL/glew.h>
#include <iostream>
#include <algorithm>
#include <cfloat>
#include <glm/gtc/matrix_transform.hpp>

Mesh::~Mesh() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Atributo de posición (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Atributo de normal (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // Atributo de color (location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    // Atributo de coordenadas UV (location 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::draw() {
    if (hasTexture && textureID != 0) {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Model::Model() : loaded(false), scale(1.0f), center(0.0f) {}

Model::~Model() {}

glm::vec3 Model::extractColorFromMaterial(aiMaterial* material) {
    if (!material) return glm::vec3(0.8f, 0.8f, 0.8f);

    aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);

    // Intentar obtener color difuso del material
    if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
        return glm::vec3(color.r, color.g, color.b);
    }

    // Intentar color especular como fallback
    if (aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color) == AI_SUCCESS) {
        return glm::vec3(color.r, color.g, color.b);
    }

    // Intentar color ambiental como fallback
    if (aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &color) == AI_SUCCESS) {
        return glm::vec3(color.r, color.g, color.b);
    }

    // Intentar color emisivo como fallback
    if (aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &color) == AI_SUCCESS) {
        return glm::vec3(color.r, color.g, color.b);
    }

    // Fallback a gris si no hay color definido
    return glm::vec3(0.8f, 0.8f, 0.8f);
}

glm::vec3 Model::getMeshColorByIndex(int meshIndex) {
    static const glm::vec3 colorPalette[] = {
        glm::vec3(1.0f, 0.2f, 0.2f),   // Rojo
        glm::vec3(0.2f, 1.0f, 0.2f),   // Verde
        glm::vec3(0.2f, 0.2f, 1.0f),   // Azul
        glm::vec3(1.0f, 1.0f, 0.2f),   // Amarillo
        glm::vec3(1.0f, 0.2f, 1.0f),   // Magenta
        glm::vec3(0.2f, 1.0f, 1.0f),   // Cyan
        glm::vec3(1.0f, 0.6f, 0.2f),   // Naranja
        glm::vec3(0.6f, 0.2f, 1.0f),   // Púrpura
    };
    const int paletteSize = sizeof(colorPalette) / sizeof(colorPalette[0]);
    return colorPalette[meshIndex % paletteSize];
}

unsigned int Model::loadTextureFromMaterial(aiMaterial* material, const aiScene* scene) {
    if (!material) return 0;

    aiString textureFile;
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFile) == AI_SUCCESS) {
        std::string texturePath = modelDirectory + textureFile.C_Str();

        std::cout << "    Intentando cargar: " << texturePath << std::endl;
        unsigned int textureID = TextureLoader::loadTextureFromFile(texturePath.c_str());

        if (textureID != 0) {
            return textureID;
        }

        // Intentar rutas alternativas
        std::string altPaths[] = {
            std::string(textureFile.C_Str()),
            modelDirectory + "../" + textureFile.C_Str(),
            modelDirectory + "../../" + textureFile.C_Str()
        };

        for (const auto& path : altPaths) {
            std::cout << "    Intentando ruta alternativa: " << path << std::endl;
            textureID = TextureLoader::loadTextureFromFile(path.c_str());
            if (textureID != 0) {
                return textureID;
            }
        }
    }

    return 0;
}

bool Model::loadModel(const char* path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_PreTransformVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error al cargar modelo: " << importer.GetErrorString() << std::endl;
        return false;
    }

    // Extraer directorio del modelo para cargar texturas
    std::string fullPath(path);
    size_t lastSlash = fullPath.find_last_of("/\\");
    modelDirectory = (lastSlash != std::string::npos) ? fullPath.substr(0, lastSlash + 1) : "";

    std::cout << "Modelo cargado: " << path << std::endl;
    std::cout << "Directorio: " << modelDirectory << std::endl;
    std::cout << "Meshes: " << scene->mNumMeshes << std::endl;

    glm::mat4 identity(1.0f);
    processNode(scene->mRootNode, scene, identity);
    normalizeModel();

    // Configurar VAO/VBO para todos los meshes
    for (auto& mesh : meshes) {
        mesh.setupMesh();
    }

    loaded = true;
    return true;
}

void Model::processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform) {
    glm::mat4 nodeTransform(1.0f);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            nodeTransform[j][i] = node->mTransformation[i][j];
        }
    }

    glm::mat4 currentTransform = parentTransform * nodeTransform;

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, currentTransform);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, currentTransform);
    }
}

void Model::processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& nodeTransform) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Extraer color del material asociado
    glm::vec3 meshColor(0.8f, 0.8f, 0.8f);
    if (mesh->mMaterialIndex < scene->mNumMaterials) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        meshColor = extractColorFromMaterial(material);
    }

    // Si el color es el gris por defecto, usar color procedural por índice de mesh
    if (meshColor == glm::vec3(0.8f, 0.8f, 0.8f)) {
        meshColor = getMeshColorByIndex(meshes.size());
    }

    // Procesar vértices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Posición con transformación de nodo
        glm::vec4 pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
        glm::vec4 transformedPos = nodeTransform * pos;
        vertex.position = glm::vec3(transformedPos) / transformedPos.w;

        // Normal con transformación de nodo
        if (mesh->HasNormals()) {
            glm::vec4 norm(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 0.0f);
            glm::vec4 transformedNorm = nodeTransform * norm;
            vertex.normal = glm::normalize(glm::vec3(transformedNorm));
        }
        else {
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        // Color del vértice o del material
        if (mesh->HasVertexColors(0)) {
            vertex.color = glm::vec3(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b);
        }
        else {
            vertex.color = meshColor;
        }

        // Coordenadas de textura
        if (mesh->HasTextureCoords(0)) {
            vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
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

    // Intentar cargar textura del material
    if (mesh->mMaterialIndex < scene->mNumMaterials) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        newMesh.textureID = loadTextureFromMaterial(material, scene);
        newMesh.hasTexture = (newMesh.textureID != 0);
    }

    std::cout << "  Mesh: " << vertices.size() << " vértices, "
        << indices.size() / 3 << " triángulos" << (newMesh.hasTexture ? " [con textura]" : "") << std::endl;

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

float Model::getRecommendedZoom() const {
    if (!loaded || scale <= 0.0f) return -4.0f;

    float inverseScale = 1.0f / scale;
    float zoomValue = -4.0f - (inverseScale * 0.3f);
    zoomValue = std::max(zoomValue, -10.0f);
    zoomValue = std::min(zoomValue, -1.5f);

    std::cout << "  Scale: " << scale << " → Zoom: " << zoomValue << std::endl;

    return zoomValue;
}

void Model::draw(class Shader* shader) {
    if (!loaded) {
        std::cerr << "Modelo no cargado\n";
        return;
    }

    for (auto& mesh : meshes) {
        // Si hay shader, pasar uniforme de textura
        if (shader != nullptr) {
            if (mesh.hasTexture && mesh.textureID != 0) {
                shader->setBool("uHasTexture", true);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mesh.textureID);
                shader->setInt("uTexture", 0);
            } else {
                shader->setBool("uHasTexture", false);
            }
        }
        mesh.draw();
    }
}
