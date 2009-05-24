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

//	动画序列枚举
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

#ifndef	GLOBAL_VARIABLE
#define	GLOBAL_VARIABLE
typedef	struct GlobalVariables
{
	//	摄像机
	//	缩放
	static float CameraZoomStep;
	static float CameraZoomBegin;
	static float CameraZoomMin;
	static float CameraZoomMax;

	//	俯仰
	static float CameraPitchMax;
	static float CameraPitchMin;
	static float CameraPitchCurrent;
	
	//	锁定
	static bool PlayerCameraLocked;
	static bool PlayerPositionLocked;

	//	THE ONE 属性
	static float TheOneHeight;

	static float TheOneJump_UT;	//	跳跃动画上升时间
	static float TheOneJump_DT;	//	跳跃动画下降时间
	static float TheOneJump_LT;	//	跳跃动画着陆时间

	static float TheOneJump_H;	//	跳跃顶点
	static float TheOneJump_h;	//	跳跃高度
	static float TheOneJump_V;	//	起跳速度
	static float TheOneJump_T;	//	跳跃时长
	static float TheOneJump_t;	//	跳跃时间
	static bool TheOneJumping;

	//	多重状态，随机切换
	static AnimationStatus TheOneMoveAS;
	static AnimationStatus TheOneAttackAS;
	static AnimationStatus TheOneIdleAS;

	/*	system settings */
	static float Gravity;
	static float UnitsPerMeter;
	static float BackSoundVolume;

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
} GV;

#endif