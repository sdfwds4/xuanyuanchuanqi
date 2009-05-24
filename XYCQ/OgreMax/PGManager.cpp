#include "PGManager.h"

PGManager::PGManager(void):
mCamera(0),
mTrees(0),mTreeLoader(0),
mBushes(0),mBushLoader(0),
mGrass(0),mGrassLoader(0),
mWorldSize(2000.0),
mShow(PM_ALL)
{
}

PGManager::~PGManager(void)
{
	deInitialise();
}

void PGManager::deInitialise()
{
	if(mTreeLoader)
	{
		delete mTreeLoader;
		mTreeLoader = 0;
	}
	if(mTrees)
	{
		delete mTrees;
		mTrees = 0;
	}
	if(mBushLoader)
	{
		delete mBushLoader;
		mBushLoader = 0;
	}
	if(mBushes)
	{
		delete mBushes;
		mBushes = 0;
	}
	if(mGrassLoader)
	{
		delete mGrassLoader;
		mGrassLoader = 0;
	}
	if(mGrass)
	{
		delete mGrass;
		mGrass = 0;
	}
}



