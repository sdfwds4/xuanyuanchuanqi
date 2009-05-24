/*	set the global variables */
#include "GlobalDefines.h"

/*	environment settings */
float GV::UnitsPerMeter = 10.0;
float GV::Gravity = GV::Meter(9.8);
float GV::BackSoundVolume = 0.2;

/*	camera zoom setting */
float GV::CameraZoomStep = GV::Meter(0.5);
float GV::CameraZoomBegin = GV::Meter(3.0);
float GV::CameraZoomMin = GV::Meter(2.0);
float GV::CameraZoomMax = GV::Meter(4.0);

/*	camera pitch setting */
float GV::CameraPitchMin = 0.0;
float GV::CameraPitchMax = 180.0;
float GV::CameraPitchCurrent = 45.0;

/*	lock player's status */
bool GV::PlayerCameraLocked = false;
bool GV::PlayerPositionLocked = false;

/*	the one's infos */
float GV::TheOneHeight = GV::Meter(1.5);
AnimationStatus GV::TheOneMoveAS = AS_WALK;
AnimationStatus GV::TheOneIdleAS = AS_IDLE;
AnimationStatus GV::TheOneAttackAS = AS_ATTACK;

/*	jumping infos */
bool GV::TheOneJumping = false;
float GV::TheOneJump_UT = 0.0;
float GV::TheOneJump_DT = 0.0;
float GV::TheOneJump_LT = 0.0;

float GV::TheOneJump_H = 0.0;
float GV::TheOneJump_h = 0.0;
float GV::TheOneJump_V = 0.0;
float GV::TheOneJump_T = 0.0;
float GV::TheOneJump_t = 0.0;