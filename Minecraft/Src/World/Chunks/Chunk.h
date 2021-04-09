#pragma once

#include "../WorldConfig.h"
#include "../Blocks/Block.h"

namespace MC {

    BRQ_ALIGN(16) class Chunk {

    private:
        Block     m_Blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH]; // Do we really want to allocate blocks on stack ????? 2 * 16 * 16 * 16 = 8KB per chunk
        BRQ::Mesh m_ChunkMesh;
        glm::vec3 m_Position;

    public:
        Chunk() = default;
        ~Chunk() = default;

        void SetBlock(BlockType type, const glm::vec3& position);
        Block GetBlock(const glm::vec3& position);

        void LoadChunk(const glm::vec3 position, const Block blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH]); 
    };
}