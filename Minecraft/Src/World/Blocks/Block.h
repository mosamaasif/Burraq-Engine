#pragma once

#include <Engine.h>

namespace MC {

    enum class BlockType : U8 {

        Air = 0,
        Grass,
        Dirt,
        Iron,
        Gold,
        Lignt,
        BlockTypeMaxEnumerations
    };


    struct Block {
        
        BlockType   Type = BlockType::Air;
        bool        IsRendered = false;
    };
}
