#pragma once
#include <stdint.h>
#include <stddef.h>
#if defined(_WIN32)
# ifdef TERRAIN_TILES_EXPORTS
#  define TERRAIN_API __declspec(dllexport)
# else
#  define TERRAIN_API __declspec(dllimport)
# endif
#else
# define TERRAIN_API __attribute__((visibility("default")))
#endif
#ifdef __cplusplus
extern "C" {
#endif
/* UTF-8 paths. Coordinates: WGS84 degrees. Heights: metres above the source
   vertical datum of the tileset, without vertical datum conversion.
   No C++ ownership or CRT allocations cross the DLL boundary. */
typedef void* TerrainTilesHandle;
#pragma pack(push,8)
typedef struct TerrainTilesView 
{
    double west, south, east, north;
    int32_t width, height;
} TerrainTilesView;

typedef struct TerrainTilesStatus 
{
    uint32_t size;
    int32_t indexing; /* 1 while indexing, 0 when complete */
    uint32_t files, rejected;
    uint64_t frameRevision, sampleRevision;
} TerrainTilesStatus;
#pragma pack(pop)
/* Create reads layer.json asynchronously. server: 0=local, 1=HTTP(S). Destroy joins the worker.
   NULL means creation failed. All other functions are nonblocking apart from
   copying the bounded result buffer under a mutex. */
TERRAIN_API TerrainTilesHandle TerrainTilesCreate(const char* rootUtf8, int server);
TERRAIN_API void TerrainTilesDestroy(TerrainTilesHandle handle);
TERRAIN_API int TerrainTilesGetStatus(TerrainTilesHandle handle,TerrainTilesStatus* status);
/* Requests supersede older requests. Invalid request => 0, valid => ticket. */
TERRAIN_API uint64_t TerrainTilesRequestView(TerrainTilesHandle handle,const TerrainTilesView* view);
/* 1=ready, 0=pending/stale, -1=invalid/insufficient buffer. BGRA premultiplied,
   top-down, width*height*4 bytes; pixels without elevation are transparent. */
TERRAIN_API int TerrainTilesCopyFrame(TerrainTilesHandle handle,uint64_t ticket,unsigned char* bgra,size_t bytes);
TERRAIN_API uint64_t TerrainTilesRequestSample(TerrainTilesHandle handle,double longitude,double latitude);
/* 1=valid height, 2=no data, 0=pending/stale, -1=invalid. */
TERRAIN_API int TerrainTilesGetSample(TerrainTilesHandle handle,uint64_t ticket,double* metres);
TERRAIN_API int TerrainTilesGetError(TerrainTilesHandle handle,char* utf8,size_t bytes);
#ifdef __cplusplus
}
#endif
