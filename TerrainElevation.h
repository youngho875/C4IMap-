#pragma once
#include <stdint.h>
#include <stddef.h>
#if defined(_WIN32)
# ifdef TERRAIN_ELEVATION_EXPORTS
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
   vertical datum (DTED/SRTM usually orthometric), never ellipsoid heights.
   No C++ ownership or CRT allocations cross the DLL boundary. */
typedef void* TerrainHandle;
#pragma pack(push,8)
typedef struct TerrainView 
{
    double west, south, east, north;
    int32_t width, height;
} TerrainView;

typedef struct TerrainStatus 
{
    uint32_t size;
    int32_t indexing; /* 1 while indexing, 0 when complete */
    uint32_t files, rejected;
    uint64_t frameRevision, sampleRevision;
} TerrainStatus;

#pragma pack(pop)
/* Create starts an asynchronous recursive scan. Destroy joins the worker.
   NULL means creation failed. All other functions are nonblocking apart from
   copying the bounded result buffer under a mutex. */
TERRAIN_API TerrainHandle TerrainCreate(const char* rootUtf8);
TERRAIN_API void TerrainDestroy(TerrainHandle handle);
TERRAIN_API int TerrainGetStatus(TerrainHandle handle,TerrainStatus* status);
/* Requests supersede older requests. Invalid request => 0, valid => ticket. */
TERRAIN_API uint64_t TerrainRequestView(TerrainHandle handle,const TerrainView* view);
/* 1=ready, 0=pending/stale, -1=invalid/insufficient buffer. BGRA premultiplied,
   top-down, width*height*4 bytes; pixels without elevation are transparent. */
TERRAIN_API int TerrainCopyFrame(TerrainHandle handle,uint64_t ticket,unsigned char* bgra,size_t bytes);
TERRAIN_API uint64_t TerrainRequestSample(TerrainHandle handle,double longitude,double latitude);
/* 1=valid height, 2=no data, 0=pending/stale, -1=invalid. */
TERRAIN_API int TerrainGetSample(TerrainHandle handle,uint64_t ticket,double* metres);
#ifdef __cplusplus
}
#endif
