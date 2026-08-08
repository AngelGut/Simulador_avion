// ============================================================
// ARCHIVO: model_loader.cpp
// DESCRIPCION: Implementación del cargador con VAO/VBO y soporte de texturas GLB
// ============================================================
#include <glad/glad.h>
#include "model_loader.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <cfloat>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

    // Atributo de coord de textura (location 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::draw() {
    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    GLint useTexLoc = glGetUniformLocation(currentProgram, "uUseTexture");

    if (hasTexture && textureID != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        if (useTexLoc != -1) glUniform1i(useTexLoc, 1);
    } else {
        if (useTexLoc != -1) glUniform1i(useTexLoc, 0);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if (hasTexture && textureID != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

Model::Model() : loaded(false), scale(1.0f), center(0.0f) {}

Model::~Model() {
    for (auto const& [key, val] : loadedTextures) {
        if (val != 0) glDeleteTextures(1, &val);
    }
}

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

bool Model::loadModel(const char* path) {
    finalTransforms.clear();
    loadedTextures.clear();
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

    // Obtener directorio del modelo para resolver texturas relativas
    std::string modelPathStr = path;
    std::string modelDir = modelPathStr.substr(0, modelPathStr.find_last_of("/\\") + 1);

    // Si tiene animaciones, extraer el último fotograma clave de cada canal
    if (scene->HasAnimations()) {
        aiAnimation* anim = scene->mAnimations[0];
        std::cout << "  Detectada animación: " << anim->mName.C_Str() << " (Duración: " << anim->mDuration << ")" << std::endl;
        
        for (unsigned int i = 0; i < anim->mNumChannels; i++) {
            aiNodeAnim* channel = anim->mChannels[i];
            std::string nodeName = channel->mNodeName.C_Str();
            
            aiVector3D scale(1.0f, 1.0f, 1.0f);
            if (channel->mNumScalingKeys > 0) {
                scale = channel->mScalingKeys[channel->mNumScalingKeys - 1].mValue;
            }
            
            aiQuaternion rotation;
            if (channel->mNumRotationKeys > 0) {
                rotation = channel->mRotationKeys[channel->mNumRotationKeys - 1].mValue;
            }
            
            aiVector3D position(0.0f, 0.0f, 0.0f);
            if (channel->mNumPositionKeys > 0) {
                position = channel->mPositionKeys[channel->mNumPositionKeys - 1].mValue;
            }
            
            aiMatrix4x4 transMat, rotMat, scaleMat;
            aiMatrix4x4::Translation(position, transMat);
            rotMat = aiMatrix4x4(rotation.GetMatrix());
            aiMatrix4x4::Scaling(scale, scaleMat);
            
            finalTransforms[nodeName] = transMat * rotMat * scaleMat;
        }
        std::cout << "  Bakeado fotograma final de animación para " << finalTransforms.size() << " nodos." << std::endl;
    }

    glm::mat4 identity(1.0f);
    processNode(scene->mRootNode, scene, identity, modelDir);
    normalizeModel();

    // Configurar VAO/VBO para todos los meshes
    for (auto& mesh : meshes) {
        mesh.setupMesh();
    }

    loaded = true;
    return true;
}

void Model::processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform, const std::string& modelDir) {
    glm::mat4 nodeTransform(1.0f);
    aiMatrix4x4 aiTrans = node->mTransformation;
    std::string nodeName = node->mName.C_Str();
    
    // Si hay una matriz bakeada para esta pieza, usarla
    if (finalTransforms.find(nodeName) != finalTransforms.end()) {
        aiTrans = finalTransforms[nodeName];
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            nodeTransform[j][i] = aiTrans[i][j];
        }
    }

    glm::mat4 currentTransform = parentTransform * nodeTransform;

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, currentTransform, modelDir);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, currentTransform, modelDir);
    }
}

void Model::processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& nodeTransform, const std::string& modelDir) {
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
        if (mesh->mTextureCoords[0]) {
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
    newMesh.textureID = 0;
    newMesh.hasTexture = false;

    // Cargar textura si el material tiene una
    if (mesh->mMaterialIndex < scene->mNumMaterials) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString path;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
            std::string texPath = path.C_Str();
            if (loadedTextures.find(texPath) != loadedTextures.end()) {
                newMesh.textureID = loadedTextures[texPath];
                newMesh.hasTexture = true;
            } else {
                unsigned int textureID = 0;
                if (texPath[0] == '*') {
                    int textureIndex = std::stoi(texPath.substr(1));
                    if (textureIndex >= 0 && textureIndex < scene->mNumTextures) {
                        aiTexture* tex = scene->mTextures[textureIndex];
                        textureID = loadEmbeddedTexture(tex);
                    }
                } else {
                    std::string fullTexPath = resolveTexturePath(modelDir, texPath);
                    textureID = loadTextureFromFile(fullTexPath);
                }
                
                if (textureID != 0) {
                    loadedTextures[texPath] = textureID;
                    newMesh.textureID = textureID;
                    newMesh.hasTexture = true;
                }
            }
        }
    }

    std::cout << "  Mesh: " << vertices.size() << " vértices, "
        << indices.size() / 3 << " triángulos" 
        << (newMesh.hasTexture ? " [Con Textura]" : "") << std::endl;

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

void Model::draw() {
    if (!loaded) {
        std::cerr << "Modelo no cargado\n";
        return;
    }

    for (auto& mesh : meshes) {
        mesh.draw();
    }
}

unsigned int Model::loadEmbeddedTexture(const aiTexture* embeddedTexture) {
    if (!embeddedTexture) return 0;

    int width, height, nrComponents;
    unsigned char* data = nullptr;

    if (embeddedTexture->mHeight == 0) {
        // Textura comprimida (PNG/JPEG)
        data = stbi_load_from_memory(
            reinterpret_cast<const unsigned char*>(embeddedTexture->pcData),
            embeddedTexture->mWidth,
            &width, &height, &nrComponents, 4
        );
    } else {
        // Textura descomprimida (ARGB8888)
        data = stbi_load_from_memory(
            reinterpret_cast<const unsigned char*>(embeddedTexture->pcData),
            embeddedTexture->mWidth * embeddedTexture->mHeight * 4,
            &width, &height, &nrComponents, 4
        );
    }

    if (!data) {
        std::cerr << "  [STB] Error al decodificar textura embebida: " << stbi_failure_reason() << std::endl;
        return 0;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}

unsigned int Model::loadTextureFromFile(const std::string& fullPath) {
    int width, height, nrComponents;
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrComponents, 4);
    if (!data) {
        return 0;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}

std::string Model::resolveTexturePath(const std::string& modelDir, const std::string& texPath) {
    std::string filename = texPath;
    size_t lastSlash = texPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        filename = texPath.substr(lastSlash + 1);
    }
    return modelDir + filename;
}
