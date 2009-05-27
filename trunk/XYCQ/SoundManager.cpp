#include "SoundManager.h"

template<>SoundManager *Singleton<SoundManager>::ms_Singleton = 0;

SoundManager::SoundManager(Ogre::Camera *cam,int chn)
{
	mCamera = cam;
	mMaxChannels = chn;
	mFmodResult = FMOD_OK;
	
	/*	create and init the sound system */
	mFmodResult = FMOD::System_Create(&mSystem);
	resultCheck();

	mFmodResult = mSystem->init(mMaxChannels,FMOD_INIT_NORMAL|FMOD_INIT_3D_RIGHTHANDED,0);
	resultCheck();

	mSystem->set3DSettings(1.0,GV::UnitsPerMeter,1.0);

	/*	create the music env and sound eff sound groups */
	mFmodResult = mSystem->createSoundGroup("music environment",&mMusicEnv);
	resultCheck();

	mFmodResult = mSystem->createSoundGroup("sound effective",&mSoundEff);
	resultCheck();

	m3DListener = new t3DListener(cam);
}

SoundManager::~SoundManager(void)
{
	if(m3DListener)
	{
		delete m3DListener;
	}
	std::vector<tNodeSound *>::iterator itr = mSoundPool.begin();
	for(;itr<mSoundPool.end();itr++)
	{
		delete (*itr);
	}
	mMusicEnv->release();
	mSoundEff->release();
	mFmodResult = mSystem->release();
	resultCheck();
	mSoundPool.clear();
}

SoundManager *SoundManager::getSingletonPtr()
{
	return ms_Singleton;
}
SoundManager SoundManager::getSingleton()
{
	assert(ms_Singleton); return (*ms_Singleton);
}

/*	create sound and add it to the sound pool */
tNodeSound *SoundManager::createSound(char *fileName,bool isLoop, bool is3D,Ogre::SceneNode *node,bool isMusicEnv,
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
	resultCheck();

	/*	judge the type of the sound */
	if(isMusicEnv)
		nodeSound->mSound->setSoundGroup(mMusicEnv);
	else
		nodeSound->mSound->setSoundGroup(mSoundEff);

	/*	add to the sound pool */
	mSoundPool.push_back(nodeSound);

	return nodeSound;
}
/*	play the sound */
void SoundManager::playSound(tNodeSound *nodeSound)
{
	mFmodResult = mSystem->playSound(FMOD_CHANNEL_FREE,nodeSound->mSound,false,&nodeSound->mChannel);
	resultCheck();
}
/*	release the sound */
void SoundManager::releaseSound(tNodeSound *nodeSound)
{
	mFmodResult = nodeSound->mChannel->stop();
	resultCheck();
	mFmodResult = nodeSound->mSound->release();
	resultCheck();

	std::vector<tNodeSound *>::iterator itr;
	itr = std::find(mSoundPool.begin(),mSoundPool.end(),nodeSound);
	mSoundPool.erase(itr);
}

void SoundManager::update()
{
	/*	update the status of listener */
	m3DListener->update();
	mFmodResult = mSystem->set3DListenerAttributes(m3DListener->id,m3DListener->mPosition,m3DListener->mVelocity,m3DListener->mForward,m3DListener->mUp);
	resultCheck();

	/*	update the 3d sound source */
	std::vector<tNodeSound *>::iterator itr = mSoundPool.begin();
	for(;itr<mSoundPool.end();itr++)
	{
		(*itr)->update();
	}

	/*	update the system, necessary for 3d sounds and virtual channels etc. */
	mFmodResult = mSystem->update();
	resultCheck();
}

void SoundManager::setMusicEnvVolume(float vol)
{
	mFmodResult = mMusicEnv->setVolume(vol);
	resultCheck();
}
void SoundManager::setSoundEffVolume(float vol)
{
	mFmodResult = mSoundEff->setVolume(vol);
	resultCheck();
}