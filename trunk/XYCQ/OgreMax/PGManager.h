/*	---------------------------- PagedGeometry Manager --------------------------------------------	

		Ŀǰ��֧�֣�Զ����������Ͻ��Ĺ�ľ��������Χ�ڵĲݣ�����ҳ�����͡�
		�ҶԲݵأ�ֻ֧��һ�㡣ϣ���Ժ�汾�ܹ�������

		\ʹ�÷�����
			1. new PGManager();

			2. ->initialise();						-->> ��ȡ��������͡������С��
			3. ->initTrees/Bushes/Grass();			-->> ����PGʵ��
			
			4. ->setTree/Bush/GrassPage();			-->> ����LOD����Ⱦ�㼶
			5. ->setTree/Bush/GrassPageLoader();	-->> ҳ���������Ĭ�������С
			
			6. ->setHeightFunction();				-->> ���ø߶�ѡ����

			7. ->addTree/Bush/Grass();				-->> ���Ԫ��
			8. ->update();							-->> ��֡����
			9. delete;								-->> �ͷ��ڴ�
		
		\������
			1. setGrassMapBounds();			-->> ���ò�ͼ��Χ����2�ı���
			2. setGrassDensityMap();		-->> �ݵ��ܶȷֲ�ͼ
			   setGrassColorMap();			-->> ����β�����Ӱ
		    
			3. setTree/BushColorMap();		-->> ͬ��

			4. setCamera();					-->> ���������������
			5. show();						-->> ��ʾģʽ������ʾ�Ľ����ٱ�����

*	-------------------------------------------------------------------------------------------- */

#ifndef _PG_MANAGER_H
#define _PG_MANAGER_H

enum PAGE_MODE
{
	PM_NONE = 0,
	PM_TREE = 1,
	PM_BUSH = 2,
	PM_GRASS = 4,
	PM_ALL = 7
};

#include <PagedGeometryAll.h>

using namespace Ogre;
using namespace Forests;

class PGManager
{
public:
	PGManager(void);
	~PGManager(void);
private:
	Camera *mCamera;
	Real mWorldSize;
	
	PAGE_MODE mShow;

	//	the trees
	PagedGeometry *mTrees;
	TreeLoader2D *mTreeLoader;
	Real mTreePageSize;
	String mTreeColorMap;
	bool mShowTrees;

	//	the bushes
	PagedGeometry *mBushes;
	TreeLoader2D *mBushLoader;
	Real mBushPageSize;
	String mBushColorMap;
	bool mShowBushes;

	//	the grass
	PagedGeometry *mGrass;
	GrassLoader *mGrassLoader;
	Real mGrassPageSize;
	String mGrassColorMap;
	String mGrassDensityMap;
	bool mShowGrass;

	GrassLayer *mGrassLayer;

public:
	/* init the world of PagedGeometry.
	\param:
		cam, the camera used in PG world;
		wsize, the size of the PG world;
	\note:
		we use the same camera here.
	*/
	inline void initialise(Camera *cam, Real wsize)
	{
		//	�����ͳһ��������PG��������������ľ���͡��ݡ�
		mCamera = cam;

		//	�����С���������á������͡���ľ��ҳ���������Χ
		mWorldSize = wsize;
	}

	/*	init the page of trees	*/
	inline void initTrees(Real pagesz, bool autoset = false)
	{
		if(mTrees) return;
		mTreePageSize = pagesz;
		mTrees = new PagedGeometry(mCamera,pagesz);
		mShow = PAGE_MODE(mShow|PM_TREE);
		if(autoset)
		{
			setTreePage(Vector2(200,50),Vector2(400,50));
			setTreePageLoader();
		}
	}

	/*	init the page of bushes	*/
	inline void initBushes(Real pagesz, bool autoset = false)
	{
		if(mBushes) return;
		mBushPageSize = pagesz;
		mBushes = new PagedGeometry(mCamera,pagesz);
		mShow = PAGE_MODE(mShow|PM_BUSH);
		if(autoset)
		{
			setBushPage(Vector2(80,30),Vector2(160,30));
			setBushPageLoader();
		}
	}

	/*	init the page of grass	*/
	inline void initGrass(Real pagesz, bool autoset = false)
	{
		if(mGrass) return;
		mGrassPageSize = pagesz;
		mGrass = new PagedGeometry(mCamera,pagesz);
		mShow = PAGE_MODE(mShow|PM_GRASS);
		if(autoset)
		{
			setGrassPage(250.0);
			setGrassPageLoader();
		}	
	}

	/*	set the height function.*/
	inline void setHeightFunction(PAGE_MODE pm,
		Real (*hf)(Real x, Real z, void *userData), void *extra = 0)
	{
		if(pm & PM_TREE)
		{
			mTreeLoader->setHeightFunction(hf);
		}
		if(pm & PM_BUSH)
		{
			mBushLoader->setHeightFunction(hf);
		}
		if(pm & PM_GRASS)
		{
			mGrassLoader->setHeightFunction(hf);
		}
	}

	/*	the following functions are used to setup the page detail level of
		the tree, bush, and grass pagedgeometry.
	\param:
		dist, up to the distance using this rendering mode;
		trans, from the dist of the camera to trans, the mode start to translated;
		autoload, if shall we auto set the page mode.
	\note:
		the tree and bush PG has the batch and impostor mode,
		the grass has been optimized in these ways, so just set the grasspage is ok.
	*/
	inline void setTreeBatchPage(Real dist, Real trans)
	{
		mTrees->addDetailLevel<BatchPage>(dist, trans);
	}
	inline void setTreeBatchPage(Vector2 &batch)
	{
		mTrees->addDetailLevel<BatchPage>(batch.x, batch.y);
	}
	inline void setTreeImpostorPage(Real dist, Real trans)
	{
		mTrees->addDetailLevel<ImpostorPage>(dist, trans);
	}
	inline void setTreeImpostorPage(Vector2 &impostor)
	{
		mTrees->addDetailLevel<ImpostorPage>(impostor.x, impostor.y);
	}
	//	����ҳ�棬��Ⱦ��εķ�Χ
	inline void setTreePage(Vector2 &batch, Vector2 &impostor)
	{
		setTreeBatchPage(batch);
		setTreeImpostorPage(impostor);
	}

	/*	set the page of bushes	*/
	inline void setBushBatchPage(Real dist, Real trans)
	{
		mBushes->addDetailLevel<BatchPage>(dist, trans);
	}
	inline void setBushBatchPage(Vector2 &batch)
	{
		mBushes->addDetailLevel<BatchPage>(batch.x, batch.y);
	}
	inline void setBushImpostorPage(Real dist, Real trans)
	{
		mBushes->addDetailLevel<ImpostorPage>(dist, trans);
	}
	inline void setBushImpostorPage(Vector2 &impostor)
	{
		mBushes->addDetailLevel<ImpostorPage>(impostor.x, impostor.y);
	}
	//	����ҳ�棬��Ⱦ��εķ�Χ
	inline void setBushPage(Vector2 &batch, Vector2 &impostor)
	{
		setBushBatchPage(batch);
		setBushImpostorPage(impostor);
	}
	
	/*	set the page of grass	*/
	inline void setGrassPage(Real dist)
	{
		mGrass->addDetailLevel<GrassPage>(dist);
	}
	
	/*	set the page loaders.
	*/
	inline void setTreePageLoader(Forests::TBounds &bounds)
	{
		mTreeLoader = new TreeLoader2D(mTrees,bounds);
		mTrees->setPageLoader(mTreeLoader);
	}
	inline void setTreePageLoader(void)
	{
		//	���ü�������Χ��Ĭ��Ϊ��ʼ���ġ������С��
		setTreePageLoader(TBounds(0,0,mWorldSize,mWorldSize));
	}

	inline void setBushPageLoader(Forests::TBounds &bounds)
	{
		mBushLoader = new TreeLoader2D(mBushes,bounds);
		mBushes->setPageLoader(mBushLoader);
	}
	inline void setBushPageLoader(void)
	{
		//	���ü�������Χ��Ĭ��Ϊ��ʼ���ġ������С��
		setBushPageLoader(TBounds(0,0,mWorldSize,mWorldSize));
	}

	inline void setGrassPageLoader(void)
	{
		mGrassLoader = new GrassLoader(mGrass);
		mGrass->setPageLoader(mGrassLoader);
	}

	/*	check the initialise status */
	inline bool hasInited(PAGE_MODE pm)
	{
		if(pm & PM_TREE)
		{
			if((mTrees == 0) || (mTreeLoader == 0))
				return false;
		}
		if(pm & PM_BUSH)
		{
			if((mBushes == 0) || (mBushLoader == 0))
				return false;
		}
		if(pm & PM_GRASS)
		{
			if((mGrass == 0) || (mGrassLoader == 0))
				return false;
		}
		return true;
	}

	/*	to add trees and bush entities here using addTree and addBush,
		you NEVER need to copy the entity yourself, it has been done internally.
		to setup the grass layer with addGrass, just one layer here.
	\param:
		ent, the entity to be added to the PG world;
		pos, yaw, scale, how to add the entity;
	\note:
		it shall be changed to allow to add multilayer of grass.
	*/
	inline void addTree(Ogre::Entity *ent, Ogre::Vector3 &pos, Ogre::Degree yaw = Degree(0), Ogre::Real scale = 1.0)
	{
		mTreeLoader->addTree(ent,pos,yaw,scale);
	}
	inline void addBush(Ogre::Entity *ent, Ogre::Vector3 &pos, Ogre::Degree yaw = Degree(0), Ogre::Real scale = 1.0)
	{
		mBushLoader->addTree(ent,pos,yaw,scale);
	}
	/*	add grass here;

	\func
			setMinimumSize(float width,float height);
			setMaximumSize(float width,float height);
			
			setSwayDistribution(float freq);	<<----->>	this will set the "positional phase shift"
												<<----->>	(λ���) of different grass;
	*/
	inline void addGrass(Ogre::String &mat,Vector2 &minsz,Vector2 &maxsz,Real dens,bool anim=true)
	{
		mGrassLayer = mGrassLoader->addLayer(mat);
		
		mGrassLayer->setMinimumSize(minsz.x, minsz.y);
		mGrassLayer->setMaximumSize(maxsz.x, maxsz.y);
		
		mGrassLayer->setAnimationEnabled(anim);
		mGrassLayer->setSwayDistribution(10.0f);
		mGrassLayer->setSwayLength(0.5f);
		mGrassLayer->setSwaySpeed(0.5f);
		mGrassLayer->setDensity(dens);
		mGrassLayer->setFadeTechnique(FADETECH_GROW);
	}

	/*	set the tree bush and grass color map, 
		in order to generate the shadow with the terrain;
	*/
	inline void setBushColorMap(String &pic)
	{
		mBushColorMap = pic;
		mBushLoader->setColorMap(mBushColorMap);
	}

	inline void setTreeColorMap(Ogre::String &pic)
	{
		mTreeColorMap = pic;
		mTreeLoader->setColorMap(mTreeColorMap);
	}

	/*	the map bounds is needed by the colormap and density map	*/
	inline void setGrassMapBounds(Forests::TBounds &bounds)
	{
		mGrassLayer->setMapBounds(bounds);
	}
	inline void setGrassColorMap(Ogre::String &pic)
	{
		mGrassColorMap = pic;
		mGrassLoader->getLayerList().front()->setColorMap(mGrassColorMap);
	}
	inline void setGrassDensityMap(String &pic)
	{
		mGrassDensityMap = pic;
		mGrassLoader->getLayerList().front()->setDensityMap(mGrassDensityMap);
	}

	/* reset the camera if necessary. */
	inline void setCamera(Camera *cam)
	{
		mCamera = cam;

		if(mTrees)
			mTrees->setCamera(cam);
		if(mBushes)
			mBushes->setCamera(cam);
		if(mGrass)
			mGrass->setCamera(cam);
	}
	/*	to switch whether show the trees, bushes or grass.
	*	used for debuging.
	*/
	inline void show(PAGE_MODE pm)
	{
		mShow = pm;
	}
	/* check the show status
	\note
		011 & 010 is true;
		100 & 010 is false;
		if use &&,until zero appears is false;
	*/
	inline bool isShown(PAGE_MODE pm)
	{
		if(pm & PM_TREE)
		{
			if(PM_TREE & mShow)return false;
		}
		if(pm & PM_BUSH)
		{
			if(PM_BUSH & mShow)return false;
		}
		if(pm & PM_GRASS)
		{
			if(PM_GRASS & mShow)return false;
		}
		/* when all the given status is ok, return true */
		return true;
	}

	/*	update each pagedgeometry when you refreshed the scene;
	*/
	inline void update()
	{
		if(mTrees && (mShow&PM_TREE))
			mTrees->update();

		if(mBushes && (mShow&PM_BUSH))
			mBushes->update();

		if(mGrass && (mShow&PM_GRASS))
			mGrass->update();
	}

	//	after all, delete the PGloader and PG here;
	void deInitialise();
};

#endif