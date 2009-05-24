#ifndef _APPLICATION_LISTENER_H
#define _APPLICATION_LISTENER_H

#include "GlobalDefines.h"
#include "SoundManager.h"
#include "GObject.h"
#include "collision.h"

//	键盘控制枚举
enum WALK_KEYS
{
	WK_W	= 1,
	WK_WD	= 3,
	WK_D	= 2,
	WK_DS	= 6,
	WK_S	= 4,
	WK_SA	= 12,
	WK_A	= 8,
	WK_AW	= 9
};
struct tKeyStatus
{
	tKeyStatus()
	{
		mWalkKey = 0;
		mDownKeys = 0;
		mRotateStep = 5.0;
		mRotate = 0;
		mGoMouseDown = false;
		mTranslate = Vector3::ZERO;
		mDestPoint = Vector3::ZERO;
	}
	void update()
	{
		//	主角朝向，需修改：
		switch(mWalkKey)
		{
		case WK_W:
			GObject::G_TheOne->mSceneNode->setDirection(Vector3(0,0,1),Node::TS_PARENT);
			break;
		case WK_WD:
			GObject::G_TheOne->mSceneNode->setDirection(Vector3(-1,0,1),Node::TS_PARENT);
			break;
		case WK_D:
			GObject::G_TheOne->mSceneNode->setDirection(Vector3(-1,0,0),Node::TS_PARENT);
			break;
		case WK_DS:
			GObject::G_TheOne->mSceneNode->setDirection(Vector3(-1,0,-1),Node::TS_PARENT);
			break;
		case WK_S:
			GObject::G_TheOne->mSceneNode->setDirection(Vector3(0,0,-1),Node::TS_PARENT);
			break;
		case WK_SA:
			GObject::G_TheOne->mSceneNode->setDirection(Vector3(1,0,-1),Node::TS_PARENT);
			break;
		case WK_A:
			GObject::G_TheOne->mSceneNode->setDirection(Vector3(1,0,0),Node::TS_PARENT);
			break;
		case WK_AW:
			GObject::G_TheOne->mSceneNode->setDirection(Vector3(1,0,1),Node::TS_PARENT);
			break;
		}
	}
	//	在状态转变的时候，重置，本来想动态锁定视角和位置的，可还是有问题。。。
	void reset()
	{
		mWalkKey = 0;
		mDownKeys = 0;
		mRotate = 0;
		mTranslate = Vector3::ZERO;
		mGoMouseDown = false;
	}
	int mWalkKey;
	int mDownKeys;
	float mRotateStep;
	float mRotate;
	bool mGoMouseDown;
	Vector3 mDestPoint;
	Vector3 mTranslate;
};

class ApplicationListener : public FrameListener,
	public OIS::KeyListener, OIS::MouseListener, Collision::CollisionListener
{
public:
	ApplicationListener(SceneManager *mgr,OIS::Keyboard *key = 0,OIS::Mouse *mouse = 0,
		OgreMax::OgreMaxScene *scn = 0,OgreOpcode::CollisionObject *obj = 0);
	~ApplicationListener();

	//	frame update
	bool frameStarted(const FrameEvent &evt);
	bool frameEnded(const FrameEvent &evt);

	//	keyboard event
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);
	bool moveKeyHandler(const OIS::KeyEvent &arg,bool isPressed = true);
	bool systemKeyHandler(const OIS::KeyEvent &arg,bool isPressed = true);
	bool numKeyHandler(const OIS::KeyEvent &arg,bool isPressed = true);

	//	mouse event
	bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mouseMoved(const OIS::MouseEvent &arg);

	//	update
	void updatePlayerStatus(const FrameEvent &evt);
	void updateMouseDownEffect(const FrameEvent &evt);
	void updateJump(const FrameEvent &evt);
	void updateStats();
	void showDebugOverlay(bool show);

	Real getTerrainHeight(Real x,Real z,void *extra=0);
private:
	void onLButtonDown(const OIS::MouseEvent &arg);
	void onRButtonDown(const OIS::MouseEvent &arg);
	void onLButtonUp(const OIS::MouseEvent &arg);
	void onRButtonUp(const OIS::MouseEvent &arg);

	SceneManager *mSceneMgr;
	RenderWindow *mWindow;
	SoundManager *mSoundMgr;
	OIS::Keyboard *mKeyboard;
	OIS::Mouse *mMouse;
	OgreMax::OgreMaxScene *mScene;

	Overlay *mDebugOverlay;

	//	some important nodes
	SceneNode *mPlayerNode;
	SceneNode *mCameraNode;
	SceneNode *mTheOneNode;
	SceneNode *mMDEffectNode;
	AnimationState *mMDEffectAS;
	OgreOpcode::CollisionContext *mCollContext;
	OgreOpcode::CollisionObject *mTheOneObject;

	//	control variables
	bool mContinue;
	tKeyStatus mKeyStatus;

	//	the ray query
	RaySceneQuery *mRaySceneQuery;
};

#endif