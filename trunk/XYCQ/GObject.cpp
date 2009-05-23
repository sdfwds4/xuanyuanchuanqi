#include "GObject.h"

GObject *GObject::G_TheOne = new GObject("TheOne");

GObject::GObject(Ogre::String name,SceneNode *node,Entity *entity)
{
	mName.assign(name);
	if(node)
	{
		mSceneNode = node;
	}
	if(entity)
	{
		mEntity = entity;
	}
	mProperties					= 0;
	mAnimation.resize(AS_NUM);

	mCurrentAnimation			= 0;
}

GObject::~GObject(void)
{
	//	释放动画指针内存
	std::vector<tSoundAnimation *>::iterator itr = mAnimation.begin();
	for(;itr<mAnimation.end();itr++)
	{
		if((*itr))
		{
			delete (*itr);
		}
	}
	if(mProperties)
	{
		delete mProperties;
	}
	if(G_TheOne)
	{
		delete G_TheOne;
	}
}

//	添加动画，无声
void GObject::addAnimation(AnimationStatus as)
{
	tSoundAnimation *tsanim;
	AnimationState *anist;
	switch(as)
	{
	case AS_IDLE:
		anist = mEntity->getAnimationState("Idle");
		break;
	case AS_IDLE1:
		anist = mEntity->getAnimationState("Idle1");
		break;
	case AS_IDLE2:
		anist = mEntity->getAnimationState("Idle2");
		break;
	case AS_WALK:
		anist = mEntity->getAnimationState("Walk");
		break;
	case AS_RUN:
		anist = mEntity->getAnimationState("Run");
		break;
	case AS_JUMP:
		anist = mEntity->getAnimationState("Jump");
		break;
	case AS_JUMP_UP:
		anist = mEntity->getAnimationState("JumpUp");
		break;
	case AS_JUMP_DOWN:
		anist = mEntity->getAnimationState("JumpDown");
		break;
	case AS_JUMP_LAND:
		anist = mEntity->getAnimationState("JumpLand");
		break;
	case AS_ATTACK:
		anist = mEntity->getAnimationState("Attack");
		break;
	case AS_ATTACK1:
		anist = mEntity->getAnimationState("Attack1");
		break;
	case AS_ATTACK2:
		anist = mEntity->getAnimationState("Attack2");
		break;
	case AS_SPELL:
		anist = mEntity->getAnimationState("Spell");
		break;
	case AS_DIE:
		anist = mEntity->getAnimationState("Die");
		break;
	}
	if(as >= AS_IDLE && as < AS_NUM)
	{
		anist->setLoop(false);
		tsanim = new tSoundAnimation(anist,as);
		mAnimation.at(as) = tsanim;
	}
}
//	添加动画，有声
void GObject::addAnimation(AnimationStatus as, char *soundFile)
{
	tSoundAnimation *tsanim;
	AnimationState *anist;
	bool isloop = false;
	switch(as)
	{
	case AS_IDLE:
		anist = mEntity->getAnimationState("Idle");
		break;
	case AS_IDLE1:
		anist = mEntity->getAnimationState("Idle1");
		break;
	case AS_IDLE2:
		anist = mEntity->getAnimationState("Idle2");
		break;
	case AS_WALK:
		anist = mEntity->getAnimationState("Walk");
		isloop = true;
		break;
	case AS_RUN:
		anist = mEntity->getAnimationState("Run");
		isloop = true;
		break;
	case AS_JUMP:
		anist = mEntity->getAnimationState("Jump");
		break;
	case AS_JUMP_UP:
		anist = mEntity->getAnimationState("JumpUp");
		break;
	case AS_JUMP_DOWN:
		anist = mEntity->getAnimationState("JumpDown");
		break;
	case AS_JUMP_LAND:
		anist = mEntity->getAnimationState("JumpLand");
		break;
	case AS_ATTACK:
		anist = mEntity->getAnimationState("Attack");
		break;
	case AS_ATTACK1:
		anist = mEntity->getAnimationState("Attack1");
		break;
	case AS_ATTACK2:
		anist = mEntity->getAnimationState("Attack2");
		break;
	case AS_SPELL:
		anist = mEntity->getAnimationState("Spell");
		break;
	case AS_DIE:
		anist = mEntity->getAnimationState("Die");
		break;
	}
	if(as >= AS_IDLE && as < AS_NUM)
	{
		anist->setLoop(isloop);
		tNodeSound *nsd = SoundManager::getSingletonPtr()->createSound(soundFile,isloop,true,
			mSceneNode,FMOD_CREATECOMPRESSEDSAMPLE);
		tsanim = new tSoundAnimation(anist,as,true,nsd);
		mAnimation.at(as) = tsanim;
	}
}

void GObject::playAnimation(AnimationStatus as, bool isloop)
{
	//	如果是当前动画，跳跃动画，或非法动画，直接返回
	if(mCurrentAnimation!=0 && mCurrentAnimation->mAnimStatus == as)
	{
		//if(as!=AS_WALK && as != AS_IDLE)
		if(mCurrentAnimation->mAnimState->hasEnded())
			mCurrentAnimation->mAnimState->setTimePosition(0.0);
		return;
	}
	if(as < AS_IDLE || as >= AS_NUM)
		return ;

	//	如果有，取消当前动画，及其声音
	if(mCurrentAnimation!=0)
	{
		mCurrentAnimation->mAnimState->setEnabled(false);
		if(mCurrentAnimation->mHasSound)
		{
			mCurrentAnimation->mSound->mChannel->stop();
		}
	}

	//	获取新的动画
	mCurrentAnimation = mAnimation.at(as);
	mCurrentAnimation->mAnimState->setEnabled(true);
	mCurrentAnimation->mAnimState->setTimePosition(0.0);
	mCurrentAnimation->mAnimState->setLoop(isloop);

	if(mAnimation.at(as)->mHasSound)
	{
		bool isplaying;
		mAnimation.at(as)->mSound->mChannel->isPlaying(&isplaying);
		if(!isplaying)
		{
			SoundManager::getSingletonPtr()->playSound(mAnimation.at(as)->mSound);
		}
	}
}
void GObject::addProperties(tObjectProperties *prop)
{
	mProperties = prop;
}
void GObject::addProperties()
{
	mProperties = new tObjectProperties();
}