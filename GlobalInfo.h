#ifndef _GLOBAL_INFO_H_
#define _GLOBAL_INFO_H_

#pragma once
/************************************************************************/
/* [5\19\2006 subifox] file paths & file names													*/
/************************************************************************/

//#define FILEPATH_NETWORK_RECORD			"d:\\C4IRTDSRec\\"
#define FILEPATH_NETWORK_RECORD		"D:\\전장상황도\\record"
#define FILEPATH_RTDS_ROOT			"D:\\전장상황도"

// Folder 작성법
// GSZ_WORKING_FOLDER가 전역 변수로 값을 저장하고 있는데 연결하여 처리하면 된다.
// 1. sprintf(szPath, "%s%s", GSZ_WORKING_FOLDER, FILEPATH_RECORD)
// 2. CString strPath = CString(GSZ_WORKING_FOLDER) + CString(FILEPATH_RECORD)

#define FILEPATH_RECORD					"\\Record\\"
#define FILEPATH_OVERLAY                "\\OverlayMap\\"
//#define FILEPATH_OVERLAY_SAVE_FOLDER	"\\OverlaySave\\"
#define FILEPATH_COMPARE_TRACK_SAVE		"\\CompareTrackSave\\"
#define FILEPATH_SAVE_FOLDER			"\\SaveFiles\\"

#define FILEPATH_ADAPTER				"\\Adapter\\"

#define FILEPATH_SDL					"\\Adapter\\SDL\\"
#define FILEPATH_BMP					"\\Adapter\\Bitmap\\"

#define FILEPATH_SYMBOL					"\\Adapter\\Bitmap\\SymbolReservoir\\"
#define FILEPATH_SYMBOL_MAP 			"\\Adapter\\Bitmap\\MapSymbols\\"
#define FILEPATH_SYMBOL_TRACK			"\\Adapter\\Bitmap\\TrackSymbolsMCRC\\"
#define FILEPATH_SYMBOL_2525			"\\Adapter\\Bitmap\\TrackSymbols2525\\"

#define FILEPATH_SPECIAL_POINT			"\\Adapter\\Bitmap\\SpecialPoint\\"

//RYU_20100713 : AMDCell dmp경로 지정
#define FILEPATH_AMD_CELL				"\\Adapter\\Bitmap\\AMDCell\\"

#define FILEPATH_TOOLBAR				"\\Adapter\\Bitmap\\ToolbarBmps\\"
#define FILEPATH_BACKGROUND				"\\Adapter\\Bitmap\\Dialog\\"
#define FILEPATH_GEOTIFF_50				"\\Adapter\\Map\\GeoTiff\\TPC_50만\\"
#define FILEPATH_GEOTIFF_100			"\\Adapter\\Map\\GeoTiff\\ONC_100만\\"
#define FILEPATH_GEOTIFF_200			"\\Adapter\\Map\\GeoTiff\\JNC_200만\\"
#define FILEPATH_DTED					"\\Adapter\\Map\\DTED\\"
#define FILEPATH_CAPTURE				"\\ScreenCapture\\"
#define FILEPATH_OVERLAY_MAP			"\\OverlayMap\\"
#define FILEPATH_CONFIG					"\\config\\"
#define FILEPATH_SYMBOL_ARMY_2525B      "\\Adapter\\Bitmap\\Army2525BSymbol\\"




#define FILENAME_DEFAULT_NORTH_CONF	"NorthData.conf"		// CONFIG
#define FILENAME_DEFAULT_ADA_CONF	"ADAData.conf"		// CONFIG
#define FILENAME_DEFAULT_OBJECT_CONF	"ObjectDefaultConfiguration.conf"		// CONFIG
#define FILENAME_DEFAULT_AREA_CONF		"AreaDefaultConfiguration.conf"			// CONFIG
#define FILENAME_DEFAULT_TRACK_CONF		"TrackDefaultConfiguration.conf"		// CONFIG
#define FILENAME_DEFAULT_MCRC_CONF		"McrcDefaultConfiguration.conf"			// CONFIG
#define FILENAME_KOREA_DTED				"KoreaDem.dat"											// DTED
#define FILENAME_DTED_BMP				"dted.bmp"													// ADAPTER
#define FILENAME_SEA_PATTEN_BMP			"SeaPattern.bmp"										// BMP
#define FILENAME_LAND_PATTEN_BMP		"LandPattern.bmp"										// BMP
#define FILENAME_2525B_MAPPING_CONF		"2525BSymbolConfiguration.conf"
#define FILENAME_MULTICAST_CONF			"Multicast.conf"                    // CONFIG
#define FILENAME_HCALLSIGN_CONF			"HCallsign.conf"					  //0607 추가 
#define FILENAME_RECORDING_CLIENT_CONF	"RecordClientConfiguration.conf" //항적 저장 클라이언트 추가 090921
#define ASRSClientFileName				"ASRSClient.exe"

//RYU_20100702 : AMD-Cell관련 Config파일 추가
#define FILENAME_DEFAULT_AMD_CELL_TRACK_CONF		"AMD-CellDefaultConfiguration.conf"		// CONFIG


/************************************************************************/
/* [5\19\2006 subifox] Window Message as user definitions
/************************************************************************/

enum EUSER_MESSAGE
{
	UM_FRAME1_FOCUS,
	UM_FRAME2_FOCUS,
};

#endif