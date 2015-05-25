#ifndef ENUMS_HPP_
#define ENUMS_HPP_

namespace terrainspace {
    enum map_layers {
        LAYER_GRASS,
        LAYER_LOWGRASS,
        LAYER_DIRT,
        LAYER_ROCK,
        LAYER_SAND,
        LAYER_COUNT,
        LAYER_LIGHTING,
        LAYER_SPECIAL,
    };


    enum grid_sides {
        GRID_FRONT,
        GRID_BACK,
        GRID_COUNT,
    };

    enum build_stage {
        // Check for the need to begin building
        STAGE_IDLE, 
        // Reset the mesh
        STAGE_CLEAR,
        // Run the quad tree and figure out what points will be in the final mesh
        STAGE_QUADTREE, 
        STAGE_TEXTURES,
        // Build the final glList
        STAGE_COMPILE,
        STAGE_WAIT_FOR_FADE,
        STAGE_DONE
    };
}

#endif
