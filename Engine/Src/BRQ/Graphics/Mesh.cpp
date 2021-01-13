#include <BRQ.h>

#include "Mesh.h"

#define FAST_OBJ_IMPLEMENTATION
#include <fast_obj.h>
#include <meshoptimizer.h>

namespace BRQ {

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<U32>& indices)
        : m_Vertices(vertices), m_Indices(indices) { }

    Mesh::Mesh(const std::string_view& filename) {

        LoadMesh(filename);
    }

    void Mesh::LoadMesh(std::string_view filename) {

        fastObjMesh* obj = fast_obj_read(filename.data());

        if (!obj) {

            BRQ_CORE_WARN("Failed to Load .obj model! Filename: {}", filename.data());
            return;
        }

        U64 totalIndices = 0;

        for (U32 i = 0; i < obj->face_count; i++)
            totalIndices += 3 * (U64)(obj->face_vertices[i] - 2);

        std::vector<Vertex> vertices(totalIndices);

        U64 vertexOffset = 0;
        U64 indexOffset = 0;

        for (U32 i = 0; i < obj->face_count; ++i) {

            for (U32 j = 0; j < obj->face_vertices[i]; ++j) {

                fastObjIndex gi = obj->indices[indexOffset + j];

                Vertex v;
                v.positions[0] =  obj->positions[gi.p * 3 + 0];
                v.positions[1] = -obj->positions[gi.p * 3 + 1];
                v.positions[2] =  obj->positions[gi.p * 3 + 2];
                v.normals[0] = obj->normals[gi.n * 3 + 0];
                v.normals[1] = obj->normals[gi.n * 3 + 1];
                v.normals[2] = obj->normals[gi.n * 3 + 2];

                if (j >= 3)
                {
                    vertices[vertexOffset + 0] = vertices[vertexOffset - 3];
                    vertices[vertexOffset + 1] = vertices[vertexOffset - 1];
                    vertexOffset += 2;
                }

                vertices[vertexOffset] = v;
                vertexOffset++;
            }

            indexOffset += obj->face_vertices[i];
        }

        fast_obj_destroy(obj);

        std::vector<unsigned int> remap(totalIndices);

        size_t total_vertices = meshopt_generateVertexRemap(&remap[0], NULL, totalIndices, &vertices[0], totalIndices, sizeof(Vertex));

        m_Indices.resize(totalIndices);
        meshopt_remapIndexBuffer(&m_Indices[0], NULL, totalIndices, &remap[0]);

        m_Vertices.resize(total_vertices);
        meshopt_remapVertexBuffer(&m_Vertices[0], &vertices[0], totalIndices, sizeof(Vertex), &remap[0]);
    }
}