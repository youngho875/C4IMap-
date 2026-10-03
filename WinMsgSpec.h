#ifndef _WIN_MSG_SPEC_H_
#define _WIN_MSG_SPEC_H_

#pragma once

enum EDBAGENT_WINDOW_MESSAGE
{
	WM_FRAME1_FOCUS		= WM_USER + 20,  
	WM_FRAME2_FOCUS		= WM_USER + 21,  

	/************************************************************************/
	/* DBAGENT
	/************************************************************************/
	DBAGENT_WM_BEGIN = WM_USER + 1001,
	DBAGENT_WM_OPERATION,

	/************************************************************************/
	/* C4I 영역 
	/************************************************************************/
	WM_C4I_MSG_BEGIN		= WM_USER + 2000,
	/************************************************************************/
	/* RTDS 영역
	/************************************************************************/
	WM_C4I_RTDS_BEGIN		= WM_C4I_MSG_BEGIN+1,
	WM_DRAW_AREAS,
	WM_INSERT_AREA,					
	WM_BRIGHTNESS_CHANGE, 	
	WM_ITO_FILTER_ONOFF,		
	WM_ITO_FILTER_CHANGE,		
	WM_ITO_FIND_ON_MAP,			
	WM_ITO_DISPLAY_TD,			
	WM_TRACK_INITIATION,		
	WM_OPTION_CHANGE,				

	WM_UPDATE_HOOK_TRACK,								// LPARAM: invalid,			WPARAM: invalid
	WM_CHANGE_HOOK_TRACK_AS_MODE2,			// LPARAM: nMode2[int],	WPARAM: invalid	
	WM_CHANGE_HOOK_TRACK_AS_NUMBER,			// LPARAM: nMode2[int],	WPARAM: invalid

	WM_SCREEN_CAPTURE_ROUTING,
	WM_UPDATE_TRACK_COMPARE,
	WM_UPDATE_RT_TRACE,
	WM_UPDATE_SAVETRK_TRACE, //0612 jskim
	WM_CHECK_BASE,
	WM_CHECK_WEAPON,		//20081107 JKS. 무장정보
	/************************************************************************/
	/* ASLS 영역
	/************************************************************************/
	WM_C4I_ASLS_BEGIN		= WM_C4I_MSG_BEGIN + 200,
	WM_ASLS_OPTION_CHANGE,

		
	/************************************************************************/
	/* 기지방호작전 프로젝트 영역
	/* 방공포병작전 프로젝트 영역
	/* Database관리 프로젝트 영역
	/************************************************************************/
	WM_AD_MSG,	
	WM_BD_MSG,	
	WM_DA_MSG,

};

#endif