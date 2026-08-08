#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace CPU {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
    };

    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    };

    struct Model {
        std::vector<MeshData> meshes;
        bool loaded = false;
    };

    bool loadModel(const std::string& path, Model& outModel);

} // namespace CPU
