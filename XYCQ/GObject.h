#pragma once
#include <vector>
#include <string>
#include "GlobalDefines.h"
#include "SoundManager.h"

//	单位属性
struct tObjectProperties
{
	tObjectProperties(int jing = 10,int qi = 10,int shen = 10,
		float attackValue = 10,float armorValue = 10)
	{
		mJing = jing;
		mQi = qi;
		mShen = shen;
		mAttackValue = attackValue;
		mArmorValue = armorValue;
		mColor = Ogre::Vector4(1,0,0,1);
	}
	//	精、气、神
	int mJing;
	int mQi;
	int mShen;

	Ogre::Vector4 mColor;

	//	攻防、速度
	float mAttackValue;
	float mArmorValue;
	float mMovingSpeed;
};

struct tSoundAnimation
{
	tSoundAnimation(AnimationState *state = 0,
		AnimationStatus as = AS_IDLE,bool hasSound = false,tNodeSound *sound = 0)
	{
		mHasSound = hasSound;
		mSound = sound;
		mAnimState = state;
		mAnimStatus = as;
	}
	tNodeSound *mSound;
	bool mHasSound;
	AnimationState *mAnimState;
	AnimationStatus mAnimStatus;
};

class GObject
{
public:
	GObject(Ogre::String name,SceneNode *node = 0,Entity *entity = 0);
	~GObject(void);
	String mName;
	Entity *mEntity;
	SceneNode *mSceneNode;
	tObjectProperties *mProperties;
public:
	static GObject *G_TheOne;
	//	用于更新当前动画时间
	tSoundAnimation *mCurrentAnimation;
	//	手动外部添加动画序列
	std::vector<tSoundAnimation *> mAnimation;
	void addAnimation(AnimationStatus as);
	//void addAnimation(AnimationStatus as,tSoundAnimation *anim);
	void addAnimation(AnimationStatus as,char *soundFile);
	void playAnimation(AnimationStatus as = AS_IDLE,bool isloop = false);

	void addProperties(tObjectProperties *prop);
	void addProperties(void);
};