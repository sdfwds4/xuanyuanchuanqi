#include "ApplicationListener.h"

ApplicationListener::ApplicationListener(SceneManager *mgr, OIS::Keyboard *key,OIS::Mouse *mouse,
										 OgreMax::OgreMaxScene *scn,OgreOpcode::CollisionObject *obj)
:mSceneMgr(mgr),
mWindow(0),
mScene(scn),
mDebugOverlay(0),
mKeyboard(key),
mMouse(mouse),
mTheOneObject(obj),
mContinue(true)
{
	mSoundMgr = SoundManager::getSingletonPtr();
	mWindow = Root::getSingletonPtr()->getAutoCreatedWindow();
	mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");

	mPlayerNode = mgr->getSceneNode("PlayerNode");
	mCameraNode = mgr->getCamera("PlayerCamera")->getParentSceneNode();
	mTheOneNode = mgr->getSceneNode("TheOneNode");
	mMDEffectNode = mgr->getSceneNode("mouseDownEffectNode");
	mMDEffectAS = mgr->getEntity("mouseDownEffectEntity")->getAnimationState("mouseDownEffect");

	mCollContext = OgreOpcode::CollisionManager::getSingletonPtr()->getDefaultContext();

	mKeyStatus.mDestPoint = mPlayerNode->getPosition();

	if(mKeyboard)
		mKeyboard->setEventCallback(this);
	if(mMouse)
		mMouse->setEventCallback(this);
}
ApplicationListener::~ApplicationListener(void)
{
}

CEGUI::MouseButton ois2CeguiMB(const OIS::MouseButtonID id)
{
	switch(id)
	{
	case OIS::MB_Left:
		return CEGUI::LeftButton;
	case OIS::MB_Right:
		return CEGUI::RightButton;
	case OIS::MB_Middle:
		return CEGUI::MiddleButton;
	}
	return CEGUI::LeftButton;
}

void ApplicationListener::onLButtonDown(const OIS::MouseEvent &arg)
{
	//	摄像机射线查询，由鼠标取得目的地
	if(mKeyStatus.mDownKeys == 0)
	{
		//CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
		//Real xPos = mousePos.d_x/float(arg.state.width);
		//Real yPos = mousePos.d_y/float(arg.state.height);
		//Ray mouseRay = mSceneMgr->getCamera("PlayerCamera")->getCameraToViewportRay(xPos,yPos);

		//mRaySceneQuery->setRay(mouseRay);
		//mRaySceneQuery->setSortByDistance(false);

		//RaySceneQueryResult &result = mRaySceneQuery->execute();
		//RaySceneQueryResult::iterator itr = result.begin( );
		//for(;itr<result.end();itr++)
		//{
		//	if(itr->worldFragment)
		//	{
		//		AnimationStatus as = GObject::G_TheOne->mCurrentAnimation->mAnimStatus;
		//		if((as == GV::TheOneIdleAS || as == GV::TheOneMoveAS) &&
		//			!GV::PlayerPositionLocked)
		//		{
		//			if(!GV::TheOneJumping)
		//				GObject::G_TheOne->playAnimation(GV::TheOneMoveAS,true);
		//			mKeyStatus.mDestPoint = itr->worldFragment->singleIntersection;
		//			mKeyStatus.mGoMouseDown = true;

		//			mMDEffectNode->setVisible(true);
		//			mMDEffectNode->setPosition(itr->worldFragment->singleIntersection.x,
		//				itr->worldFragment->singleIntersection.y + 10,
		//				itr->worldFragment->singleIntersection.z);
		//			mMDEffectAS->setTimePosition(01);
		//		}
		//		break;
		//	}
		//}
	}
}

void ApplicationListener::onRButtonDown(const OIS::MouseEvent &arg)
{
	CEGUI::MouseCursor::getSingletonPtr()->hide();
}
void ApplicationListener::onLButtonUp(const OIS::MouseEvent &arg)
{
}
void ApplicationListener::onRButtonUp(const OIS::MouseEvent &arg)
{
	CEGUI::MouseCursor::getSingletonPtr()->show();
}
// 更新玩家摄像机视角
void ApplicationListener::updatePlayerStatus(const FrameEvent &evt)
{
	//	地形射线查询，决定视角高度
	Vector3 pos = mPlayerNode->getPosition();
	Real height = GV::getTerrainHeight(pos.x,pos.z);
	pos.y = height + GV::TheOneHeight/2.0;

	mPlayerNode->setPosition(pos);

	//	旋转视角
	mPlayerNode->yaw( -Degree(mKeyStatus.mRotate), Node::TS_WORLD);
	GObject::G_TheOne->mSceneNode->yaw( Degree(mKeyStatus.mRotate), Node::TS_WORLD);

	AnimationStatus as = GObject::G_TheOne->mCurrentAnimation->mAnimStatus;

	//	mCollContext->collide();
	//	键盘移动
	if(mKeyStatus.mDownKeys > 0)
	{
		if(as != GV::TheOneAttackAS && as != AS_SPELL && as != AS_JUMP_LAND)
		{
			mKeyStatus.update();
			if(as != GV::TheOneIdleAS)
			{
				Vector3 dist = mKeyStatus.mTranslate * evt.timeSinceLastFrame;
				//Vector3 pos = mPlayerNode->getPosition();
				//Vector3 vel = mPlayerNode->getOrientation()*dist;

				//Vector3 des = Collision::collideAndSlide(pos,vel,10,Vector3::ZERO,this);
				//mPlayerNode->setPosition(des);
				mPlayerNode->translate(dist,Node::TS_LOCAL);
			}
		}
	}
	////	鼠标移动
	//if(mKeyStatus.mDownKeys == 0 && mKeyStatus.mGoMouseDown)
	//{
	//	Vector3 dir = mKeyStatus.mDestPoint - mPlayerNode->getPosition();
	//	dir.y = 0;
	//	//	鼠标点取位置未达到，朝向并移动
	//	if(dir != Vector3::ZERO)
	//	{
	//		Real dist = dir.normalise();
	//		GObject::G_TheOne->mSceneNode->setDirection(dir,Node::TS_WORLD);
	//		
	//		// 需修改，主角模型朝向
	//		GObject::G_TheOne->mSceneNode->yaw(Degree(180));
	//		if(dist < GObject::G_TheOne->mProperties->mMovingSpeed/10.0)
	//		{
	//			mPlayerNode->setPosition(mKeyStatus.mDestPoint);
	//		}
	//		else
	//		{
	//			//Vector3 dist = dir * GObject::G_TheOne->mProperties->mMovingSpeed * evt.timeSinceLastFrame;
	//			//Vector3 pos = mPlayerNode->getPosition();
	//			//Vector3 vel = mPlayerNode->getOrientation()*dist;

	//			//Vector3 des = Collision::collideAndSlide(pos,vel,1,Vector3::ZERO,this);
	//			//mPlayerNode->setPosition(des);
	//			mPlayerNode->translate(dir * GObject::G_TheOne->mProperties->mMovingSpeed * evt.timeSinceLastFrame,Node::TS_WORLD);
	//		}
	//	}
	//	//	已到达鼠标点选位置，休息
	//	else
	//	{
	//		mKeyStatus.mGoMouseDown = false;
	//		if(!GV::TheOneJumping)
	//			GObject::G_TheOne->playAnimation(GV::TheOneIdleAS,false);
	//	}
	//}
	//	当前动作结束，且无跳跃，进行随机发呆动作
	if(GObject::G_TheOne->mCurrentAnimation->mAnimState->hasEnded() && !GV::TheOneJumping)
	{
		if(mKeyStatus.mDownKeys == 0)
		{
			if(as == AS_IDLE1 || as == AS_IDLE2)
			{
				GV::TheOneIdleAS = AnimationStatus(AS_IDLE + rand()%3);
			}
			else if(rand()%3 == 0)
			{
				GV::TheOneIdleAS = AnimationStatus(AS_IDLE + rand()%3);
			}
			if(!GV::TheOneJumping)
				GObject::G_TheOne->playAnimation(GV::TheOneIdleAS,false);
		}
		else
		{
			GObject::G_TheOne->playAnimation(GV::TheOneMoveAS,true);
		}
	}

	if(GV::TheOneJumping)
	{
		mTheOneNode->setPosition(Vector3(0,GV::TheOneJump_h,0));
	}
	// 动画更新
	GObject::G_TheOne->mCurrentAnimation->mAnimState->addTime(evt.timeSinceLastFrame);
}
void ApplicationListener::updateMouseDownEffect(const FrameEvent &evt)
{
	if(mMDEffectAS->hasEnded())
	{
		mMDEffectNode->setVisible(false);
	}
	else
	{
		mMDEffectAS->addTime(4*evt.timeSinceLastFrame);
	}
}
void ApplicationListener::updateJump(const FrameEvent &evt)
{
	float t = GV::TheOneJump_t;

	if(t >= 0 && t < GV::TheOneJump_UT)
	{
		GObject::G_TheOne->playAnimation(AS_JUMP_UP);
	}
	else if(t >= GV::TheOneJump_T/2.0 && t < GV::TheOneJump_T/2.0 + GV::TheOneJump_DT)
	{
		GObject::G_TheOne->playAnimation(AS_JUMP_DOWN);
	}
	else if(t >= GV::TheOneJump_T - GV::TheOneJump_LT && t < GV::TheOneJump_T)
	{
		GObject::G_TheOne->playAnimation(AS_JUMP_LAND);
	}
	else if(t > GV::TheOneJump_T)
	{
		GV::TheOneJumping = false;
		mTheOneNode->setPosition(0,0,0);
		mKeyStatus.mGoMouseDown = false;
		return ;
	}
	GV::TheOneJump_t += evt.timeSinceLastFrame;
	GV::TheOneJump_h = GV::TheOneJump_V*GV::TheOneJump_t - 
		GV::Gravity * GV::TheOneJump_t * GV::TheOneJump_t / 2.0;	
}

bool ApplicationListener::frameStarted(const FrameEvent &evt)
{
	mKeyboard->capture();
	mMouse->capture();

	//	状态更新
	mCollContext->collide(evt.timeSinceLastFrame);
	
	updatePlayerStatus(evt);
	updateMouseDownEffect(evt);
	
	//	跳跃重力作用更新
	if(GV::TheOneJumping)
	{
		updateJump(evt);
	}

	//	更新max场景
	if(mScene)
		mScene->Update(evt.timeSinceLastFrame);

	//	更新3D声音
	if(mSoundMgr)
		mSoundMgr->update();

	return mContinue;
}
bool ApplicationListener::frameEnded(const FrameEvent &evt)
{
	updateStats();
	return true;
}
bool ApplicationListener::numKeyHandler(const OIS::KeyEvent &arg, bool isPressed)
{
	AnimationStatus as = GObject::G_TheOne->mCurrentAnimation->mAnimStatus;
	if(isPressed)
	{
		switch(arg.key)
		{
		case OIS::KC_1:
			mKeyStatus.mGoMouseDown = false;
			if(as != AS_ATTACK && as != AS_ATTACK1 && as != AS_ATTACK2)
			{
				GV::TheOneAttackAS = AnimationStatus(AS_ATTACK + rand()%3);
				if(!GV::TheOneJumping)
					GObject::G_TheOne->playAnimation(GV::TheOneAttackAS);
			}
			break;
		case OIS::KC_2:
			mKeyStatus.mGoMouseDown = false;
			if(GObject::G_TheOne->mCurrentAnimation->mAnimStatus != AS_SPELL)
			{
				if(!GV::TheOneJumping)
					GObject::G_TheOne->playAnimation(AS_SPELL);
			}
			break;
		case OIS::KC_3:
			{
			}
			break;
		}
	}
	return true;
}
bool ApplicationListener::systemKeyHandler(const OIS::KeyEvent &arg,bool isPressed)
{
	if(isPressed)
	{
		AnimationStatus as = GObject::G_TheOne->mCurrentAnimation->mAnimStatus;
		switch(arg.key)
		{
		case OIS::KC_A:
			if(!GV::PlayerCameraLocked)
				mKeyStatus.mRotate -= mKeyStatus.mRotateStep;
			break;
		case OIS::KC_D:
			if(!GV::PlayerCameraLocked)
				mKeyStatus.mRotate += mKeyStatus.mRotateStep;
			break;
		case OIS::KC_F:
			if(mDebugOverlay->isVisible())
				mDebugOverlay->hide();
			else
				mDebugOverlay->show();
			break;
		case OIS::KC_SPACE:
			if(as != AS_JUMP && as != AS_JUMP_UP && as!= AS_JUMP_DOWN && as != AS_JUMP_LAND)
			{
				GV::InitJump(GV::TheOneHeight,false);
				GV::TheOneJumping = true;
			}
			break;
		case OIS::KC_ESCAPE:
			mContinue = false;
			break;
		case OIS::KC_F1:
			/*	lock the position of player */
			GV::PlayerPositionLocked = !GV::PlayerPositionLocked;
			if(!GV::TheOneJumping)
				GObject::G_TheOne->playAnimation(GV::TheOneIdleAS,false);
			break;
		case OIS::KC_F2:
			/*	lock the camera absolutely */
			GV::PlayerCameraLocked = !GV::PlayerCameraLocked;
			break;
		case OIS::KC_F3:
			/*	lock the camera pitching */
			GV::PlayerCameraPicthLocked = !GV::PlayerCameraPicthLocked;
			break;
		case OIS::KC_R:
			AnimationStatus as = GObject::G_TheOne->mCurrentAnimation->mAnimStatus;
			//	移动状态不能同时切换移动模式
			if(GV::TheOneMoveAS == AS_WALK && as != AS_WALK && as != AS_RUN)
			{
				GV::TheOneMoveAS = AS_RUN;
				GObject::G_TheOne->mProperties->mMovingSpeed *= 2.0;
			}
			else if(GV::TheOneMoveAS == AS_RUN && as != AS_WALK && as != AS_RUN)
			{
				GV::TheOneMoveAS = AS_WALK;
				GObject::G_TheOne->mProperties->mMovingSpeed /= 2.0;
			}
			break;
		}
	}
	else
	{
		switch(arg.key)
		{
		case OIS::KC_A:
			if(!GV::PlayerCameraLocked)
				mKeyStatus.mRotate += mKeyStatus.mRotateStep;
			break;
		case OIS::KC_D:
			if(!GV::PlayerCameraLocked)
				mKeyStatus.mRotate -= mKeyStatus.mRotateStep;
			break;
		}
	}
	return true;
}

bool ApplicationListener::moveKeyHandler(const OIS::KeyEvent &arg, bool isPressed)
{
	AnimationStatus as = GObject::G_TheOne->mCurrentAnimation->mAnimStatus;
	float	movestep = GObject::G_TheOne->mProperties->mMovingSpeed;
	switch(arg.key)
	{
	case OIS::KC_W:
	case OIS::KC_UP:
		if(isPressed)
		{
			mKeyStatus.mTranslate.z -= movestep;
			mKeyStatus.mDownKeys ++;
			mKeyStatus.mWalkKey += WK_W;
		}
		else
		{
			mKeyStatus.mTranslate.z += movestep;
			mKeyStatus.mDownKeys --;
			mKeyStatus.mWalkKey -= WK_W;
		}
		break;
	case OIS::KC_S:
	case OIS::KC_DOWN:
		if(isPressed)
		{
			mKeyStatus.mTranslate.z += movestep;
			mKeyStatus.mDownKeys ++;
			mKeyStatus.mWalkKey += WK_S;
		}
		else
		{
			mKeyStatus.mTranslate.z -= movestep;
			mKeyStatus.mDownKeys --;
			mKeyStatus.mWalkKey -= WK_S;
		}
		break;
	case OIS::KC_Q:
	case OIS::KC_LEFT:
		if(isPressed)
		{
			mKeyStatus.mTranslate.x -= movestep;
			mKeyStatus.mDownKeys ++;
			mKeyStatus.mWalkKey += WK_A;
		}
		else
		{
			mKeyStatus.mTranslate.x += movestep;
			mKeyStatus.mDownKeys --;
			mKeyStatus.mWalkKey -= WK_A;
		}
		break;
	case OIS::KC_E:
	case OIS::KC_RIGHT:
		if(isPressed)
		{
			mKeyStatus.mTranslate.x += movestep;
			mKeyStatus.mDownKeys ++;
			mKeyStatus.mWalkKey += WK_D;
		}
		else
		{
			mKeyStatus.mTranslate.x -= movestep;
			mKeyStatus.mDownKeys --;
			mKeyStatus.mWalkKey -= WK_D;
		}
		break;
	}
	//*//////////////////////////////////////////////////////////////////////////////
	//*
	//*		只要是非移动态，鼠标移动便要取消，比如,Jump,Attack,Spell,Die。。。
	//*
	//*//////////////////////////////////////////////////////////////////////////////
	
	if(mKeyStatus.mDownKeys > 0)
	{
		//	只要有按键移动，无论成功与否，都取消鼠标移动
		mKeyStatus.mGoMouseDown = false;

		//	有按键，且空闲时，才可转为移动态
		if(as == GV::TheOneIdleAS)
		{
			if(!GV::TheOneJumping)
				GObject::G_TheOne->playAnimation(GV::TheOneMoveAS,true);
		}
	}
	else
	{
		//if(as != GV::MoveStyle)
		//{
		//	mKeyStatus.mGoMouseDown = false;
		//}
		//	非鼠标点取，则取消移动
		if(as == GV::TheOneMoveAS && !mKeyStatus.mGoMouseDown)
		{
			if(!GV::TheOneJumping)
				GObject::G_TheOne->playAnimation(GV::TheOneIdleAS,false);
		}
	}
	return true;
}bool ApplicationListener::keyPressed(const OIS::KeyEvent &arg)
{
	if(CEGUI::System::getSingletonPtr()->injectKeyDown(arg.key) || 
		CEGUI::System::getSingletonPtr()->injectChar(arg.text))
	{
		return true;
	}
	if(!GV::PlayerPositionLocked)
	{
		moveKeyHandler(arg);
		numKeyHandler(arg);
	}
	systemKeyHandler(arg);
	return true;
}

bool ApplicationListener::keyReleased(const OIS::KeyEvent &arg)
{
	if(CEGUI::System::getSingletonPtr()->injectKeyUp(arg.key))
	{
		return true;
	}
	if(!GV::PlayerPositionLocked)
	{
		moveKeyHandler(arg,false);
		numKeyHandler(arg,false);
	}
	systemKeyHandler(arg,false);	
	return true;
}

bool ApplicationListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(CEGUI::System::getSingletonPtr()->injectMouseButtonDown(ois2CeguiMB(id)))
	{
		return true;
	}
	if(id == OIS::MB_Left)
	{
		onLButtonDown(arg);
	}
	else if(id == OIS::MB_Right)
	{
		onRButtonDown(arg);
	}
	return true;
}

bool ApplicationListener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(CEGUI::System::getSingletonPtr()->injectMouseButtonUp(ois2CeguiMB(id)))
	{
		return true;
	}
	if(id == OIS::MB_Left)
	{
		onLButtonUp(arg);
	}
	else if(id == OIS::MB_Right)
	{
		onRButtonUp(arg);
	}
	return true;
}

bool ApplicationListener::mouseMoved(const OIS::MouseEvent &arg)
{
	if(CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel))
	{
		return true;
	}
	if(!GV::PlayerCameraLocked)
	{
		//	移动摄像机
		Vector3 camPos = mSceneMgr->getCamera("PlayerCamera")->getPosition();
		if(arg.state.Z.rel > 0 && camPos.y > GV::CameraZoomMin)
		{
			camPos.y -= GV::CameraZoomStep;
			camPos.z -= GV::CameraZoomStep;
		}
		else if(arg.state.Z.rel < 0 && camPos.y < GV::CameraZoomMax)
		{
			camPos.y += GV::CameraZoomStep;
			camPos.z += GV::CameraZoomStep;
		}
		mSceneMgr->getCamera("PlayerCamera")->setPosition(camPos);

		if(arg.state.buttonDown(OIS::MB_Right))
		{
			float stepX = 0,stepY = 0;
			if(arg.state.X.rel < 0)
			{
				stepX = mKeyStatus.mRotateStep;
			}
			else if(arg.state.X.rel>0)
			{
				stepX = -mKeyStatus.mRotateStep;
			}
			if(arg.state.Y.rel < 0)
			{
				stepY = mKeyStatus.mRotateStep;
			}
			else if(arg.state.Y.rel > 0)
			{
				stepY = -mKeyStatus.mRotateStep;
			}
			mPlayerNode->yaw( Degree(stepX), Node::TS_WORLD);

			/*	if the camera picthing is not locked */
			if(GV::PlayerCameraPicthLocked == false)
			{
				if((GV::CameraPitchCurrent > GV::CameraPitchMin && stepY < 0) ||
					(GV::CameraPitchCurrent < GV::CameraPitchMax && stepY > 0))
				{
					mCameraNode->pitch( Degree(stepY), Node::TS_LOCAL);
					GV::CameraPitchCurrent += stepY;

					/*	set the visible of the one if the camera is too low */
					if(GV::CameraPitchCurrent >= 120.0)
						mTheOneNode->setVisible(false);
					else
						mTheOneNode->setVisible(true);
				}
			}

			GObject::G_TheOne->mSceneNode->yaw( -Degree(stepX), Node::TS_WORLD);
		}
	}
	return true;
}

void ApplicationListener::updateStats(void)
{
	static String currFps = "Current FPS: ";
	static String avgFps = "Average FPS: ";
	static String bestFps = "Best FPS: ";
	static String worstFps = "Worst FPS: ";
	static String tris = "Triangle Count: ";
	static String batches = "Batch Count: ";

	// update stats when necessary
	try {
		OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
		OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
		OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
		OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

		const RenderTarget::FrameStats& stats = mWindow->getStatistics();
		guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
		guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
		guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
			+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
		guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
			+" "+StringConverter::toString(stats.worstFrameTime)+" ms");

		OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

		OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
		guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));

		//OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
		//guiDbg->setCaption(mDebugText);
	}
	catch(...) { /* ignore */ }
}
void ApplicationListener::showDebugOverlay(bool show)
{
	if(mDebugOverlay)
	{
		if(show)
		{
			mDebugOverlay->show();
		}
		else
		{
			mDebugOverlay->hide();
		}
	}
}