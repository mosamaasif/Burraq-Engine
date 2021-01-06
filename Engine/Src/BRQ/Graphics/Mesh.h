#pragma once

#include <BRQ.h>

namespace BRQ {

    struct Vertex {

        F32 positions[3];
        F32 normals[3];
    };

    class Mesh {

    private:
        std::vector<Vertex> m_Vertices;
        std::vector<U32> m_Indices;

    public:
        Mesh() = default;
        Mesh(const std::vector<Vertex>& m_Vertices, const std::vector<U32>& m_Indices);
        Mesh(const std::string_view& filename);
        ~Mesh() = default;

        void LoadMesh(std::string_view filename);

        const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
        const std::vector<U32>& GetIndices() const { return m_Indices; }
    };
}