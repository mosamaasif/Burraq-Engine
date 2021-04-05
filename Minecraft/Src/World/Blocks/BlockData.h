#pragma once

#include <Engine.h>

#include "../WorldConfig.h"

// CCW { 0, 2, 1, 2, 3, 1 }

static const F32 FrontFace[20] = {

    -BLOCK_SIZE, -BLOCK_SIZE,  BLOCK_SIZE, 0.0f, 0.0f,
    -BLOCK_SIZE,  BLOCK_SIZE,  BLOCK_SIZE, 0.0f, 1.0f,
     BLOCK_SIZE, -BLOCK_SIZE,  BLOCK_SIZE, 1.0f, 0.0f,
     BLOCK_SIZE,  BLOCK_SIZE,  BLOCK_SIZE, 1.0f, 1.0f,
};

static const F32 BackFace[20] = {

    -BLOCK_SIZE, -BLOCK_SIZE, -BLOCK_SIZE, 1.0f, 0.0f,
     BLOCK_SIZE, -BLOCK_SIZE, -BLOCK_SIZE, 0.0f, 0.0f,
    -BLOCK_SIZE,  BLOCK_SIZE, -BLOCK_SIZE, 1.0f, 1.0f,
     BLOCK_SIZE,  BLOCK_SIZE, -BLOCK_SIZE, 0.0f, 1.0f,
};

static const F32 LeftFace[20] = {

    -BLOCK_SIZE, -BLOCK_SIZE, -BLOCK_SIZE, 0.0f, 0.0f,
    -BLOCK_SIZE,  BLOCK_SIZE, -BLOCK_SIZE, 0.0f, 1.0f,
    -BLOCK_SIZE, -BLOCK_SIZE,  BLOCK_SIZE, 1.0f, 0.0f,
    -BLOCK_SIZE,  BLOCK_SIZE,  BLOCK_SIZE, 1.0f, 1.0f,
};

static const F32 RightFace[20] = {

     BLOCK_SIZE, -BLOCK_SIZE,  BLOCK_SIZE, 0.0f, 0.0f,
     BLOCK_SIZE,  BLOCK_SIZE,  BLOCK_SIZE, 0.0f, 1.0f,
     BLOCK_SIZE, -BLOCK_SIZE, -BLOCK_SIZE, 1.0f, 0.0f,
     BLOCK_SIZE,  BLOCK_SIZE, -BLOCK_SIZE, 1.0f, 1.0f,
};

static const F32 TopFace[20] = {

    -BLOCK_SIZE,  BLOCK_SIZE,  BLOCK_SIZE, 0.0f, 0.0f,
    -BLOCK_SIZE,  BLOCK_SIZE, -BLOCK_SIZE, 0.0f, 1.0f,
     BLOCK_SIZE,  BLOCK_SIZE,  BLOCK_SIZE, 1.0f, 0.0f,
     BLOCK_SIZE,  BLOCK_SIZE, -BLOCK_SIZE, 1.0f, 1.0f,
};

static const F32 BottomFace[20] = {

    -BLOCK_SIZE, -BLOCK_SIZE, -BLOCK_SIZE, 0.0f, 0.0f,
    -BLOCK_SIZE, -BLOCK_SIZE,  BLOCK_SIZE, 0.0f, 1.0f,
     BLOCK_SIZE, -BLOCK_SIZE, -BLOCK_SIZE, 1.0f, 0.0f,
     BLOCK_SIZE, -BLOCK_SIZE,  BLOCK_SIZE, 1.0f, 1.0f,
};