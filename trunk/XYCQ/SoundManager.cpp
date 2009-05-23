#include "SoundManager.h"

SoundManager *SoundManager::ms_Instance = 0;

SoundManager::SoundManager(Ogre::Camera *cam,int chn)
{
	mMaxChannels				= chn;
	mFmodResult					= FMOD_OK;
	
	//	��������ʼ������ϵͳ
	mFmodResult = FMOD::System_Create(&mSystem);
	resultCheck(mFmodResult);
	mFmodResult = mSystem->init(mMaxChannels,FMOD_INIT_NORMAL|FMOD_INIT_3D_RIGHTHANDED,0);
	resultCheck(mFmodResult);
	mSystem->set3DSettings(1.0,10.0,0.5);

	m3DListener = new t3DListener(cam);
}

SoundManager::~SoundManager(void)
{
	mFmodResult = mSystem->release();
	resultCheck(mFmodResult);

	if(m3DListener)
	{
		delete m3DListener;
	}
	std::vector<tNodeSound *>::iterator itr = mSoundPool.begin();
	for(;itr<mSoundPool.end();itr++)
	{
		delete (*itr);
	}
	mSoundPool.clear();
}

SoundManager* SoundManager::createInstance(Ogre::Camera *cam,int chn)
{
	if(0 == ms_Instance)
	{
		ms_Instance = new SoundManager(cam,chn);
	}
	return ms_Instance;
}

SoundManager *SoundManager::getSingletonPtr()
{
	return ms_Instance;
}
SoundManager SoundManager::getSingleton()
{
	return *ms_Instance;
}

//	��������
tNodeSound *SoundManager::createSound(char *fileName,bool isLoop, bool is3D,Ogre::SceneNode *node,
									  FMOD_MODE createType,FMOD_MODE relative,FMOD_MODE rolloff)
{
	tNodeSound *nodeSound = new tNodeSound(node,is3D);

	FMOD_MODE	mode = 0;
	if(isLoop)
		mode |= FMOD_LOOP_NORMAL;
	else
		mode |= FMOD_LOOP_OFF;
	if(is3D)
		mode |= FMOD_3D;
	else
		mode |= FMOD_2D;
	if(createType == FMOD_CREATECOMPRESSEDSAMPLE)
		mode |= FMOD_SOFTWARE;
	else
		mode |= FMOD_HARDWARE;

	mFmodResult = mSystem->createSound(fileName,mode|createType|relative|rolloff,0,&nodeSound->mSound);
	resultCheck(mFmodResult);
	
	mSoundPool.push_back(nodeSound);

	return nodeSound;
}
//	��������
void SoundManager::playSound(tNodeSound *nodeSound)
{
	mFmodResult = mSystem->playSound(FMOD_CHANNEL_FREE,nodeSound->mSound,false,&nodeSound->mChannel);
	resultCheck(mFmodResult);
}
//	�ͷ�����
void SoundManager::releaseSound(tNodeSound *nodeSound)
{
	mFmodResult = nodeSound->mChannel->stop();
	resultCheck(mFmodResult);
	mFmodResult = nodeSound->mSound->release();
	resultCheck(mFmodResult);

	std::vector<tNodeSound *>::iterator itr;
	itr = std::find(mSoundPool.begin(),mSoundPool.end(),nodeSound);
	mSoundPool.erase(itr);
}

//	��������
void SoundManager::update()
{
	//	����
	m3DListener->update();
	mFmodResult = mSystem->set3DListenerAttributes(m3DListener->id,m3DListener->mPosition,m3DListener->mVelocity,m3DListener->mForward,m3DListener->mUp);
	resultCheck(mFmodResult);

	//	��Դ
	std::vector<tNodeSound *>::iterator itr = mSoundPool.begin();
	for(;itr<mSoundPool.end();itr++)
	{
		(*itr)->update();
	}

	//	ϵͳ
	mFmodResult = mSystem->update();
	resultCheck(mFmodResult);
}