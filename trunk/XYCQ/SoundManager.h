#pragma once
#include "GlobalDefines.h"
#include <vector>
#include <algorithm>

// 把OGRE里的3D矢量转到FMOD中
static FMOD_VECTOR *GetFV3(Ogre::Vector3 v3)
{
	FMOD_VECTOR *fv = new FMOD_VECTOR();
	fv->x = v3.x;
	fv->y = v3.y;
	fv->z = v3.z;
	return fv;
}
static FMOD_VECTOR *GetFV3(Ogre::Vector3 *v3)
{
	return GetFV3(*v3);
}
static void SetFV3(FMOD_VECTOR *fv,Ogre::Vector3 v3)
{
	if(fv==0)
	{
		fv = new FMOD_VECTOR();
	}
	fv->x = v3.x;
	fv->y = v3.y;
	fv->z = v3.z;
}
static void SetFV3(FMOD_VECTOR *fv,Ogre::Vector3 *v3)
{
	return SetFV3(fv,*v3);
}
/*						内存管理
 *
 *	SceneNode	——>	mSceneMgr		——>	OGRE		< 存放声音位置 >
 *	Sound		——>	SoundManager	——>	FMOD		< 读取声音文件 >
 *	Channel		——>	SoundManager	——>	FMOD		< 播放声音实例 >
 *
 */

//	模型节点	< —— >	声源管理器

//	不要从此new 创建声音实例，要从全局的SoundManager实例创建
struct tNodeSound
{
	tNodeSound(Ogre::SceneNode *node = 0,bool is3d = true)
	{
		mNode = node;
		mSound = 0;
		mChannel = 0;
		mIs3D = is3d;
		
		if(node)
			mPosition = GetFV3(mNode->_getDerivedPosition());
		else
			mPosition = GetFV3(Ogre::Vector3::ZERO);
		mVelocity = GetFV3(Ogre::Vector3::ZERO);
	}
	~tNodeSound()
	{
		if(mPosition)
			delete mPosition;
		if(mVelocity)
			delete mVelocity;
	}
	void update()
	{
		if(mIs3D)
		{
			//////	需修改 ！！！
			SetFV3(mPosition,mNode->_getDerivedPosition());
			//SetFV3(mVelocity,Ogre::Vector3::ZERO);
			mChannel->set3DAttributes(mPosition,mVelocity);
		}
	}
	Ogre::SceneNode *mNode;
	FMOD_VECTOR *mPosition;
	FMOD_VECTOR *mVelocity;
	Sound *mSound;
	Channel *mChannel;
	bool mIs3D;
};


//	3D控件的监听者，位置信息靠摄像机维护
struct t3DListener
{
	t3DListener(Ogre::Camera *cam)		//	cam不能为0！！
	{
		id = 0;
		mCamera = cam;
		mPosition = GetFV3(cam->getRealPosition());
		mVelocity = GetFV3(Ogre::Vector3::ZERO);
		mForward = GetFV3(-cam->getRealDirection());
		mUp = GetFV3(cam->getRealUp());
	}
	~t3DListener()
	{
		if(mPosition)
		{
			delete mPosition;
		}
		if(mVelocity)
		{
			delete mVelocity;
		}
		if(mForward)
		{
			delete mForward;
		}
		if(mUp)
		{
			delete mUp;
		}
	}
	void update()
	{
		SetFV3(mPosition,mCamera->getRealPosition());
		SetFV3(mVelocity,Ogre::Vector3::ZERO);
		SetFV3(mForward,-mCamera->getRealDirection());
		SetFV3(mUp,mCamera->getRealUp());
	}
	int id;
	Ogre::Camera *mCamera;
	FMOD_VECTOR *mPosition;
	FMOD_VECTOR *mVelocity;
	FMOD_VECTOR *mForward;
	FMOD_VECTOR *mUp;
};
class SoundManager
{
public:
	void resultCheck(FMOD_RESULT res)
	{
		if(res != FMOD_OK)
		{
			exit(-1);
		}
	}
	tNodeSound* createSound(char *fileName,bool isLoop = false, bool is3D = true,
		Ogre::SceneNode *node = 0,							//	模型节点
		FMOD_MODE createType = FMOD_CREATECOMPRESSEDSAMPLE,	//	文件加载方式
		FMOD_MODE relative = FMOD_3D_WORLDRELATIVE,			//	3D坐标相关								
		FMOD_MODE rolloff = FMOD_3D_LOGROLLOFF);			//	衰减方式

	void playSound(tNodeSound *nodeSound);
	void releaseSound(tNodeSound *nodeSound);
	void update();

	//	外部创建与访问方法
	static SoundManager* createInstance(Ogre::Camera *cam = 0,int chn = 100);
	static SoundManager* getSingletonPtr();
	static SoundManager getSingleton();
private:
	System *mSystem;
	t3DListener *m3DListener;
	FMOD_RESULT mFmodResult;
	int mMaxChannels;
	std::vector<tNodeSound *> mSoundPool;

	//	单实例与其构建
	static SoundManager *ms_Instance;
	SoundManager(Ogre::Camera *cam = 0,int chn = 100);
	~SoundManager(void);
};