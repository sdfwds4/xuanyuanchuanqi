#pragma once
#include <Ogre.h>
#include <OIS/OIS.h>
#include <CEGUI/CEGUI.h>
#include <OgreCEGUIRenderer.h>
#include <inc/fmod.hpp>

#include <OgreOpcode.h>

#include "OgreMax/OgreMax.h"

using namespace OgreOpcode;
using namespace OgreMax;
using namespace FMOD;
//using namespace Game;

//	��������ö��
enum AnimationStatus
{
	AS_IDLE = 0,
	AS_IDLE1,
	AS_IDLE2,

	AS_WALK,
	AS_RUN,

	AS_JUMP,
	AS_JUMP_UP,
	AS_JUMP_DOWN,
	AS_JUMP_LAND,
	
	AS_ATTACK,
	AS_ATTACK1,
	AS_ATTACK2,
	
	AS_SPELL,
	AS_HURT,
	AS_DIE,
	AS_NUM
};

//	the ray query
static Ogre::RaySceneQuery *raySceneQuery;

#ifndef	GLOBAL_VARIABLE
#define	GLOBAL_VARIABLE
typedef	struct GlobalVariables
{
	//	�����
	//	����
	static float CameraZoomStep;
	static float CameraZoomBegin;
	static float CameraZoomMin;
	static float CameraZoomMax;

	//	����
	static float CameraPitchMax;
	static float CameraPitchMin;
	static float CameraPitchCurrent;
	
	//	����
	static bool PlayerCameraLocked;
	static bool PlayerCameraPicthLocked;
	static bool PlayerPositionLocked;

	//	THE ONE ����
	static float TheOneHeight;

	static float TheOneJump_UT;	//	��Ծ��������ʱ��
	static float TheOneJump_DT;	//	��Ծ�����½�ʱ��
	static float TheOneJump_LT;	//	��Ծ������½ʱ��

	static float TheOneJump_H;	//	��Ծ����
	static float TheOneJump_h;	//	��Ծ�߶�
	static float TheOneJump_V;	//	�����ٶ�
	static float TheOneJump_T;	//	��Ծʱ��
	static float TheOneJump_t;	//	��Ծʱ��
	static bool TheOneJumping;

	//	����״̬������л�
	static AnimationStatus TheOneMoveAS;
	static AnimationStatus TheOneAttackAS;
	static AnimationStatus TheOneIdleAS;

	/*	system settings */
	static float Gravity;
	static float UnitsPerMeter;
	static float MusicEnvVolume;
	static float SoundEffVolume;

	static inline float Meter(float meter)
	{
		return  meter * GV::UnitsPerMeter;
	}

	static void InitJump(float height,bool isDown = false)
	{
		TheOneJump_H = height;
		TheOneJump_V = sqrt(2*Gravity*height);
		TheOneJump_T = 2*sqrt(2*height/Gravity);

		if(isDown)
		{
			TheOneJump_t = TheOneJump_T/2.0;
		}
		else
		{
			TheOneJump_t = 0;
		}
	}
	static Real getTerrainHeight(Real x,Real z,void *extra=0)
	{
		Ray ray(Vector3(x,5000.0f,z),Vector3::NEGATIVE_UNIT_Y);
		raySceneQuery->setRay(ray);
		RaySceneQueryResult &rs = raySceneQuery->execute();
		RaySceneQueryResult::iterator itr = rs.begin();
		
		Real height = -1;
		for(;itr<rs.end();itr++)
		{
			if(itr->worldFragment)
			{
				height = itr->worldFragment->singleIntersection.y;
				break;
			}
		}
		//raySceneQuery->clearResults();
		return height;
	}
} GV;

#endif