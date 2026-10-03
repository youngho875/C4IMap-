#pragma once

#pragma warning(disable:4293)

// 공역필터를 위한 필터 플래그 값설정   07.02.24 : YHJ
class DisplayElementFilter 
{
public:
	enum Flags
	{
		//// 화생방 예상오염도
		//CM_POL1						    = 1 << 0,		// 예상 오염도 1
		//CM_POL2						    = 1 << 1,		// 예상 오염도 2
		//CM_POL3						    = 1 << 2,		// 예상 오염도 3
		//CM_POLLUTON_ALL    			    = 1 << 3,		// All
	 // 
		//// 화생방 예상오염도                   	       		
		//CM_POL4							= 1 << 4,		// 공대지 사격장
		//CM_POL5							= 1 << 5,		// 공대공 사격장
		//CM_MINUTE						= 1 << 6,		// 정밀			
		//CM_POLLUTION_ALL_1 	  			= 1 << 7,		// All
	 // 
		//// 이동전력               	      
		//CM_EOD							= 1 << 8,		// EOD
		//CM_								= 1 << 9,		// 정훈실
		//CM_CIV							= 1 << 10,		// 장갑차
		//CM_ARMY       					= 1 << 11,		// 무장 순찰차
		//CM_NAVY							= 1 << 12,		// 정찰차
		//CM_US_MIL						= 1 << 13,		// 제독차
		//CM_AMB							= 1 << 14,		// A.M.D
		//CM_FIRE							= 1 << 15,		// 소방차
		//CM_VIC_ALL						= 1 << 16,		// All
		//
		//// 배경지도
		//CM_VIDEO_WALL					= 1 << 17,		// 기지외곽선
		//CM_PEACE_TIME					= 1 << 18,		// 1/2/3지대 경계선
		//CM_WAR_TIME						= 1 << 19,		// 초소
		//CM_WAR_TIME1					= 1 << 20,		// 출립문
		//CM_WAR_TIME2					= 1 << 21,		// 전투진지
		//CM_WAR_TIME3					= 1 << 22,		// 활주로
		//CM_WAR_TIME_MRR					= 1 << 23,		// 유도로
		//CM_WAR_TIME_HIWAY               = 1 << 24,		// 비상활주로
		//CM_WAR_TIME_SPR					= 1 << 25,		// 주기장
		//CM_WAR_TIME_SHRIMP              = 1 << 26,		// 격납고
		//CM_WAR_TIME_SCALLOP             = 1 << 27,		// 도로
		//CM_WAR_TIME_CVOA				= 1 << 28,		// 탄약고
		//CM_WAR_TIME_ETC					= 1 << 29,		// 유류저장소
		//CM_VIDEO_WALL1					= 1 << 30,		// 통신시설
		//CM_PEACE_TIME1					= 1 << 31,		// 작전지휘소/배행대
		//CM_WAR_TIME4					= 1 << 32,		// 장비/보급시설
		//CM_WAR_TIME5					= 1 << 33,		// 대공방어 포상
		//CM_WAR_TIME6					= 1 << 34,		// 주요장비
		//CM_WAR_TIME7					= 1 << 35,		// 기지방어 구역
		//CM_WAR_TIME_MRR1				= 1 << 36,		// 화생방 대피구역
		//CM_WAR_TIME_HIWAY1              = 1 << 37,		// 집단보호 시설
		//CM_WAR_TIME_SPR1				= 1 << 38,		// 화생방 탐지대
		//CM_WAR_TIME_SHRIMP1             = 1 << 39,		// 화생방 자동 경보
		//CM_BACK_MAP_ALL		            = 1 << 40,		// ALL


		//CM_LOCALSYM_ALL					= 1 << 41,		// 기지심볼
		//CM_LOCALSYM_GAS					= 1 << 42,		// 가스
		//CM_LOCALSYM_ENGAGE				= 1 << 43,		// 교전
		//CM_LOCALSYM_REPAIR				= 1 << 44,		// 복구
		//CM_LOCALSYM_MISFIRE				= 1 << 45,		// 불발탄
		//CM_LOCALSYM_EXPLO				= 1 << 46,		// 폭파
		//CM_LOCALSYM_BOMBED				= 1 << 47,		// 피폭
		//CM_LOCALSYM_FIRE				= 1 << 48,		// 화재
		//CM_LOCALSYM_CRATER				= 1 << 49,		// 활주로폭파구

		//CM_AREA_ALL						= 1 << 50,		//지역
		//CM_AREA_MOPP					= 1 << 51,		//화생방대피구역
		CM_MAP							 = 1 << 0,		//지도		
		//CM_MAP_LAYER1 					 = 1 << 1 ,		//1지대경계선			
		//CM_MAP_LAYER2 					 = 1 << 2 ,		//2지대경계선			
		//CM_MAP_LAYER3 					 = 1 << 3 ,		//3지대경계선	
		//CM_MAP_LAYER4 					 = 1 << 4 ,		//기지외곽선	
		//CM_MAP_LAYER5 					 = 1 << 5 ,		//출입문		
		//CM_MAP_LAYER6 					 = 1 << 6 ,		//전투진지
		//CM_MAP_LAYER7 					 = 1 << 7 ,		//대공방어포상	
		//CM_MAP_LAYER8 					 = 1 << 8 ,		//활주로		
		//CM_MAP_LAYER9 					 = 1 << 9 ,		//비상활주로		
		//CM_MAP_LAYER10					 = 1 << 10,		//유도로	
		//CM_MAP_LAYER11					 = 1 << 11,		//일반도로	
		//CM_MAP_LAYER12					 = 1 << 12,		//주기장		
		//CM_MAP_LAYER13					 = 1 << 13,		//격납고		
		//CM_MAP_LAYER14					 = 1 << 14,		//탄약고		
		//CM_MAP_LAYER15					 = 1 << 15,		//유류고	
		//CM_MAP_LAYER16					 = 1 << 16,		//작전지휘소		
		//CM_MAP_LAYER17					 = 1 << 17,		//비행대대	
		//CM_MAP_LAYER18					 = 1 << 18,		//정비시설	
		//CM_MAP_LAYER19					 = 1 << 19,		//통신시설				
		//CM_MAP_LAYER20					 = 1 << 20,		//보급시설	
				
		CM_MAP_LAYER1 					 = 1 << 1 ,		//1지대경계선			
		CM_MAP_LAYER2 					 = 1 << 2 ,		//2지대경계선			
		CM_MAP_LAYER3 					 = 1 << 3 ,		//3지대경계선	
		CM_MAP_LAYER4 					 = 1 << 4 ,		//기지외곽선	
		CM_MAP_LAYER5 					 = 1 << 5 ,		//출입문		
		CM_MAP_LAYER6 					 = 1 << 7 ,		//대공방어포상	
		CM_MAP_LAYER7 					 = 1 << 8 ,		//활주로		
		CM_MAP_LAYER8 					 = 1 << 9 ,		//비상활주로		
		CM_MAP_LAYER9					 = 1 << 10,		//유도로	
		CM_MAP_LAYER10					 = 1 << 11,		//일반도로	
		CM_MAP_LAYER11					 = 1 << 12,		//주기장		
		CM_MAP_LAYER12					 = 1 << 13,		//격납고		
		CM_MAP_LAYER13					 = 1 << 14,		//탄약고		
		CM_MAP_LAYER14					 = 1 << 15,		//유류고	
		CM_MAP_LAYER15					 = 1 << 16,		//작전지휘소		
		CM_MAP_LAYER16					 = 1 << 17,		//비행대대	
		CM_MAP_LAYER17					 = 1 << 19,		//통신시설	

		CM_BASE							 = 1 << 21,		//기지방호				
		CM_BASE_OUTAREA					 = 1 << 22,		//외곽방어대구역
		CM_BASE_DEFAREA					 = 1 << 23,		//지역방어대구역	
		CM_BASE_ARMORED_UNIT			 = 1 << 24,		//전투장갑차
		CM_BASE_ARMED_UNIT				 = 1 << 25,		//무장순찰차		
		CM_BASE_GUARD					 = 1 << 26,		//초소
		CM_BASE_SPT						 = 1 << 27,		//특작군
		CM_BASE_ENGAGE					 = 1 << 28,		//교전	
		CM_BASE_JAIL					 = 1 << 29,		//영창				
		CM_BASE_TEMPPRISON				 = 1 << 30,		//임시포로수집소		
		CM_BASE_SEEHIGH					 = 1 << 31,		//감제고지
		CM_BASE_SEEGUARD				 = 1 << 32,		//목진지
		CM_BASE_SEEPOINT				 = 1 << 33,		//관측소
		CM_BASE_INSPECT					 = 1 << 34,		//검문소
		CM_BASE_SPECIAL_PART			 = 1 << 35,		//특임반
		CM_BASE_ENEMY_MORTAR			 = 1 << 36,		//박격포(적)
		CM_BASE_BATTLE_CAMP				 = 1 << 37,		//전투진지
		CM_BASE_TEXT					 = 1 << 38,		//텍스트
		CM_BASE_OBJNAME					 = 1 << 39,		//객체명

		CM_NBC							 = 1 << 40,		//화생방
		CM_NBC_SHELTERAREA				 = 1 << 41,		//화학대피구역
		CM_NBC_POLLUTION				 = 1 << 42,		//오염도
		CM_NBC_POLLUTION_GUESS			 = 1 << 43,		//예측오염도
		CM_NBC_POLLUTION_DETAIL			 = 1 << 44,		//정밀오염도
		CM_NBC_POLLUTION_REAL			 = 1 << 45,		//실오염도
		CM_NBC_DETEC_UNIT				 = 1 << 46,		//화생방정찰차
		CM_NBC_DETOXIC_UNIT				 = 1 << 47,		//제독차
		CM_NBC_FIRE_UNIT				 = 1 << 48,		//소방차
		CM_NBC_FIRSTAID_UNIT			 = 1 << 49,		//구급차
		CM_NBC_EOD						 = 1 << 50,		//EOD
		CM_NBC_PROTECT					 = 1 << 51,		//집단보호시설
		CM_NBC_DETECT					 = 1 << 52,		//화학탐지대
		CM_NBC_ALARM					 = 1 << 53,		//자동경보기
		CM_NBC_DETOXIC					 = 1 << 54,		//제독소
		CM_NBC_MAINBUILDING				 = 1 << 55,		//주요건물
		CM_NBC_FIRE						 = 1 << 56,		//화재
		CM_NBC_EXPLOSION				 = 1 << 57,		//폭파	
		CM_NBC_UNEXPBOMB				 = 1 << 58,		//불발탄
		CM_NBC_GAS						 = 1 << 59,		//GAS
		CM_NBC_REPAIR					 = 1 << 60,		//복구
		CM_NBC_TEXT						 = 1 << 61,		//텍스트
		CM_NBC_OBJNAME					 = 1 << 62,		//객체명

		CM_AIRDEF						 = 1 << 63,		//대공방어
		CM_AIRDEF_PARTAREA				 = 1 << 64,		//8방향
		CM_AIRDEF_ADB_PET				 = 1 << 65,		//페트리어트
		CM_AIRDEF_ADB_VAL				 = 1 << 66,		//발칸
		CM_AIRDEF_ADB_SIN				 = 1 << 67,		//신궁
		CM_AIRDEF_ADB_MIS				 = 1 << 68,		//미스트랄
		CM_AIRDEF_HIGH					 = 1 << 69,		//고속기(적)
		CM_AIRDEF_LOW					 = 1 << 70,		//저속기(적)
		CM_AIRDEF_HELICOPTER			 = 1 << 71,		//헬기(적)
		CM_AIRDEF_TEXT					 = 1 << 72,		//텍스트
		CM_AIRDEF_OBJNAME				 = 1 << 73,		//객체명


		CM_COMMON						 = 1 << 74,		//공통
		CM_COMMON_TEXT					 = 1 << 75,		//텍스트
												

	};

	enum troop
	{
		//// 화생방 예상오염도
		//CM_POL_troop1					= 100,		// 예상 오염도 1
		//CM_POL_troop2						    ,		// 예상 오염도 2
		//CM_POL_troop3						    ,		// 예상 오염도 3
		//CM_POLLUTON_ALL_troop    			    ,		// All
	 // 
		//// 화생방 예상오염도                   		
		//CM_POL_troop4							,		// 공대지 사격장
		//CM_POL_troop5							,		// 공대공 사격장
		//CM_MINUTE_troop						,		// 정밀			
		//CM_POLLUTION_ALL_1_troop 	  			,		// All
	 // 
		//// 이동전력               	    
		//CM_EOD_troop							,		// EOD
		//CM__troop								,		// 정훈실
		//CM_CIV_troop							,		// 장갑차
		//CM_ARMY_troop       					,		// 무장 순찰차
		//CM_NAVY_troop							,		// 정찰차
		//CM_US_MIL_troop						,		// 제독차
		//CM_AMB_troop							,		// A.M.D
		//CM_FIRE_troop							,		// 소방차
		//CM_VIC_ALL_troop						,		// All
		//
		//// 배경지도
		//CM_VIDEO_WALL_troop					,		// 기지외곽선
		//CM_PEACE_TIME_troop					,		// 1/2/3지대 경계선
		//CM_WAR_TIME_troop						,		// 초소
		//CM_WAR_TIME_troop1					,		// 출립문
		//CM_WAR_TIME_troop2					,		// 전투진지
		//CM_WAR_TIME_troop3					,		// 활주로
		//CM_WAR_TIME_MRR_troop					,		// 유도로
		//CM_WAR_TIME_HIWAY_troop               ,		// 비상활주로
		//CM_WAR_TIME_SPR_troop					,		// 주기장
		//CM_WAR_TIME_SHRIMP_troop              ,		// 격납고
		//CM_WAR_TIME_SCALLOP_troop             ,		// 도로
		//CM_WAR_TIME_CVOA_troop				,		// 탄약고
		//CM_WAR_TIME_ETC_troop					,		// 유류저장소
		//CM_VIDEO_WALL_troop1					,		// 통신시설
		//CM_PEACE_TIME_troop1					,		// 작전지휘소/배행대
		//CM_WAR_TIME_troop4					,		// 장비/보급시설
		//CM_WAR_TIME_troop5					,		// 대공방어 포상
		//CM_WAR_TIME_troop6					,		// 주요장비
		//CM_WAR_TIME_troop7					,		// 기지방어 구역
		//CM_WAR_TIME_MRR_troop1				,		// 화생방 대피구역
		//CM_WAR_TIME_HIWAY_troop1              ,		// 집단보호 시설
		//CM_WAR_TIME_SPR_troop1				,		// 화생방 탐지대
		//CM_WAR_TIME_SHRIMP_troop1             ,		// 화생방 자동 경보
		//CM_BACK_MAP_ALL_troop		            ,		// ALL



		//CM_LOCALSYM_ALL_troop					,		// 기지심볼
		//CM_LOCALSYM_GAS_troop						,		// 가스
		//CM_LOCALSYM_ENGAGE_troop					,		// 교전
		//CM_LOCALSYM_REPAIR_troop					,		// 복구
		//CM_LOCALSYM_MISFIRE_troop					,		// 불발탄
		//CM_LOCALSYM_EXPLO_troop					,		// 폭파
		//CM_LOCALSYM_BOMBED_troop					,		// 피폭
		//CM_LOCALSYM_FIRE_troop					,		// 화재
		//CM_LOCALSYM_CRATER_troop					,		// 활주로폭파구


		//CM_AREA_ALL_troop                          , //지역
		//CM_AREA_MOPP_troop							, //화생방대피구역
		
		T_CM_MAP						 = 100,		//지도		
		//T_CM_MAP_LAYER1 					 ,		//1지대경계선			
		//T_CM_MAP_LAYER2 					 ,		//2지대경계선			
		//T_CM_MAP_LAYER3 					 ,		//3지대경계선	
		//T_CM_MAP_LAYER4 					 ,		//기지외곽선	
		//T_CM_MAP_LAYER5 					 ,		//출입문		
		//T_CM_MAP_LAYER6 					 ,		//전투진지
		//T_CM_MAP_LAYER7 					 ,		//대공방어포상	
		//T_CM_MAP_LAYER8 					 ,		//활주로		
		//T_CM_MAP_LAYER9 					 ,		//비상활주로		
		//T_CM_MAP_LAYER10					 ,		//유도로	
		//T_CM_MAP_LAYER11					 ,		//일반도로	
		//T_CM_MAP_LAYER12					 ,		//주기장		
		//T_CM_MAP_LAYER13					 ,		//격납고		
		//T_CM_MAP_LAYER14					 ,		//탄약고		
		//T_CM_MAP_LAYER15					 ,		//유류고	
		//T_CM_MAP_LAYER16					 ,		//작전지휘소		
		//T_CM_MAP_LAYER17					 ,		//비행대대	
		//T_CM_MAP_LAYER18					 ,		//정비시설	
		//T_CM_MAP_LAYER19					 ,		//통신시설				
		//T_CM_MAP_LAYER20					 ,		//보급시설	
			
		T_CM_MAP_LAYER1 					 ,		//1지대경계선			
		T_CM_MAP_LAYER2 					 ,		//2지대경계선			
		T_CM_MAP_LAYER3 					 ,		//3지대경계선	
		T_CM_MAP_LAYER4 					 ,		//기지외곽선	
		T_CM_MAP_LAYER5 					 ,		//출입문		 
		T_CM_MAP_LAYER6 					 ,		//대공방어포상	
		T_CM_MAP_LAYER7 					 ,		//활주로		
		T_CM_MAP_LAYER8 					 ,		//비상활주로		
		T_CM_MAP_LAYER9						 ,		//유도로	
		T_CM_MAP_LAYER10					 ,		//일반도로	
		T_CM_MAP_LAYER11					 ,		//주기장		
		T_CM_MAP_LAYER12					 ,		//격납고		
		T_CM_MAP_LAYER13					 ,		//탄약고		
		T_CM_MAP_LAYER14					 ,		//유류고	
		T_CM_MAP_LAYER15					 ,		//작전지휘소		
		T_CM_MAP_LAYER16					 ,		//비행대대	 
		T_CM_MAP_LAYER17					 ,		//통신시설	 


		T_CM_BASE							 ,		// 기지방호				
		T_CM_BASE_OUTAREA					 ,		// 외곽방어대구역
		T_CM_BASE_DEFAREA					 ,		// 지역방어대구역	
		T_CM_BASE_ARMORED_UNIT				 ,		// 전투장갑차
		T_CM_BASE_ARMED_UNIT				 ,		// 무장순찰차		
		T_CM_BASE_GUARD						 ,		// 초소
		T_CM_BASE_SPT						 ,		// 특작군
		T_CM_BASE_ENGAGE					 ,		// 교전	
		T_CM_BASE_JAIL						 ,		// 영창				
		T_CM_BASE_TEMPPRISON				 ,		// 임시포로수집소		
		T_CM_BASE_SEEHIGH					 ,		// 감제고지
		T_CM_BASE_SEEGUARD					 ,		// 목진지
		T_CM_BASE_SEEPOINT					 ,		// 관측소
		T_CM_BASE_INSPECT					 ,		// 검문소
		T_CM_BASE_SPECIAL_PART				 ,		// 특임반
		T_CM_BASE_ENEMY_MORTAR				 ,		// 박격포(적)
		T_CM_BASE_BATTLE_CAMP				 ,		// 전투진지
		T_CM_BASE_TEXT						 ,		// 텍스트
		T_CM_BASE_OBJNAME					 ,		// 객체명
	
		T_CM_NBC							 ,		// 화생방
		T_CM_NBC_SHELTERAREA				 ,		// 화학대피구역
		T_CM_NBC_POLLUTION					 ,		// 오염도
		T_CM_NBC_POLLUTION_GUESS			 ,		// 예측오염도
		T_CM_NBC_POLLUTION_DETAIL			 ,		// 정밀오염도
		T_CM_NBC_POLLUTION_REAL				 ,		// 실오염도
		T_CM_NBC_DETEC_UNIT					 ,		// 화생방정찰차
		T_CM_NBC_DETOXIC_UNIT				 ,		// 제독차
		T_CM_NBC_FIRE_UNIT					 ,		// 소방차
		T_CM_NBC_FIRSTAID_UNIT				 ,		// 구급차
		T_CM_NBC_EOD						 ,		// EOD
		T_CM_NBC_PROTECT					 ,		// 집단보호시설
		T_CM_NBC_DETECT						 ,		// 화학탐지대
		T_CM_NBC_ALARM						 ,		// 자동경보기
		T_CM_NBC_DETOXIC					 ,		// 제독소
		T_CM_NBC_MAINBUILDING				 ,		// 주요건물
		T_CM_NBC_FIRE						 ,		// 화재
		T_CM_NBC_EXPLOSION					 ,		// 폭파	
		T_CM_NBC_UNEXPBOMB					 ,		// 불발탄
		T_CM_NBC_GAS						 ,		// GAS
		T_CM_NBC_REPAIR						 ,		// 복구
		T_CM_NBC_TEXT						 ,		// 텍스트
		T_CM_NBC_OBJNAME					 ,		// 객체명
	
		T_CM_AIRDEF							 ,		// 대공방어
		T_CM_AIRDEF_PARTAREA				 ,		// 8방향
		T_CM_AIRDEF_ADB_PET					 ,		// 페트리어트
		T_CM_AIRDEF_ADB_VAL					 ,		// 발칸
		T_CM_AIRDEF_ADB_SIN					 ,		// 신궁
		T_CM_AIRDEF_ADB_MIS					 ,		// 미스트랄
		T_CM_AIRDEF_HIGH					 ,		// 고속기(적)
		T_CM_AIRDEF_LOW						 ,		// 저속기(적)
		T_CM_AIRDEF_HELICOPTER				 ,		// 헬기(적)
		T_CM_AIRDEF_TEXT					 ,		// 텍스트
		T_CM_AIRDEF_OBJNAME					 ,		// 객체명
	
		T_CM_COMMON							 ,		// 공통
		T_CM_COMMON_TEXT					 ,		// 텍스트

	};
};

