#pragma once
// Set in Project Properties > C/C++ > Preprocessor Definitions.
#ifndef C4MAP_ENABLE_LOCAL
#define C4MAP_ENABLE_LOCAL 1
#endif
#ifndef C4MAP_ENABLE_OGC
#define C4MAP_ENABLE_OGC 1
#endif
#if !C4MAP_ENABLE_LOCAL && !C4MAP_ENABLE_OGC
#error Enable at least one map source backend.
#endif

// Terrain backends follow the existing Local / Network deployment by default.
#ifndef C4MAP_ENABLE_TERRAIN_LOCAL
#define C4MAP_ENABLE_TERRAIN_LOCAL C4MAP_ENABLE_LOCAL
#endif
#ifndef C4MAP_ENABLE_TERRAIN_SERVER
#define C4MAP_ENABLE_TERRAIN_SERVER C4MAP_ENABLE_OGC
#endif

// 0: honor XML (both), 1: georeferenced original files only, 2: XYZ only.
#ifndef C4MAP_RASTER_MODE
#define C4MAP_RASTER_MODE 0
#endif
#ifndef C4MAP_CIB_MODE
#define C4MAP_CIB_MODE 0
#endif
#if C4MAP_RASTER_MODE < 0 || C4MAP_RASTER_MODE > 2 || C4MAP_CIB_MODE < 0 || C4MAP_CIB_MODE > 2
#error Map input mode must be 0 (XML), 1 (original raster), or 2 (XYZ).
#endif

// 0: both, 1: original SHP, 2: MVT vector tiles.
#ifndef C4MAP_SHAPE_MODE
#define C4MAP_SHAPE_MODE 1
#endif
#if C4MAP_SHAPE_MODE < 0 || C4MAP_SHAPE_MODE > 2
#error C4MAP_SHAPE_MODE must be 0, 1 or 2.
#endif

// Tile-only map management. Base maps and elevation sampling remain available.
#ifndef C4MAP_TILE_TREE_ONLY
#define C4MAP_TILE_TREE_ONLY 1
#endif
#if C4MAP_TILE_TREE_ONLY
#undef C4MAP_SHAPE_MODE
#define C4MAP_SHAPE_MODE 2
#undef C4MAP_RASTER_MODE
#define C4MAP_RASTER_MODE 2
#undef C4MAP_CIB_MODE
#define C4MAP_CIB_MODE 2
#endif