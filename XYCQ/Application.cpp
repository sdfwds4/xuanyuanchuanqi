#include "Application.h"
#include "HeightFunction.h"

Application::Application(void):
mKeyboard(0),mMouse(0),mInputManager(0),mListener(0),
mSoundMgr(0),mRoot(0),mWindow(0),mSceneMgr(0),mCamera(0),
mScene(0),mCollContext(0),mTheOneObject(0),
mBackSound(0),mParticle(0),
mCEGUIRenderer(0),mCEGUISystem(0),mCEGUISheet(0)
{
}

Application::~Application(void)
{
	//	CEGUI
	if(mCEGUISheet)
	{
		CEGUI::WindowManager::getSingletonPtr()->destroyWindow(mCEGUISheet);
	}
	if(mCEGUISystem)
	{
		delete mCEGUISystem;
		mCEGUISystem = 0;
	}
	if(mCEGUIRenderer)
	{
		delete mCEGUIRenderer;
		mCEGUIRenderer = 0;
	}

	if(mParticle)
	{
		mSceneMgr->destroyParticleSystem(mParticle);
	}
	// OIS
	if(mKeyboard)
	{
		mInputManager->destroyInputObject(mKeyboard);
		mKeyboard = 0;
	}
	if(mMouse)
	{
		mInputManager->destroyInputObject(mMouse);
		mMouse = 0;
	}
	if(mInputManager)
	{
		OIS::InputManager::destroyInputSystem(mInputManager);
		mInputManager = 0;
	}
	if(mScene)
	{
		delete mScene;
		mScene = 0;
	}

	// 系统
	if(mListener)
	{
		delete mListener;
		mListener = 0;
	}
	if(mRoot)
	{
		delete mRoot;
		mRoot = 0;
	}
}

void Application::windowResized(Ogre::RenderWindow *rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void Application::windowClosed(RenderWindow *rw)
{
	if( rw == mRoot->getAutoCreatedWindow())
	{	
		if(mKeyboard)
		{
			mInputManager->destroyInputObject(mKeyboard);
			mKeyboard = 0;
		}
		if(mMouse)
		{
			mInputManager->destroyInputObject(mMouse);
			mMouse = 0;
		}
		if(mInputManager)
		{
			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}

// 运行程序
void Application::go()
{
	srand(time(0));
	createRoot();
	defineResources();
	setupRenderSystem();
	createRenderWindow();
	initializeResourceGroups();
	createSceneManager();

	TextureManager::getSingleton().setDefaultNumMipmaps(5);

	setupInputSystem();
	setupCEGUI();
	setupSoundManager();
	setupCollision();
	
	createTheOne();
	createFrameListener();

	createScene();
	//createCollision();
	startRenderLoop();
}

// 创建Root对象
void Application::createRoot()
{
	mRoot = new Root();
}

// 定义资源
void Application::defineResources()
{
	String						secName,typeName,archName;
	ConfigFile					cf;
	cf.load("resources.cfg");
	
	ConfigFile::SectionIterator	secIter = cf.getSectionIterator();
	while(secIter.hasMoreElements())
	{
		secName = secIter.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = secIter.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for(i=settings->begin();i!=settings->end();++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingletonPtr()->addResourceLocation(archName,typeName,secName);
		}
	}
}

// 设置渲染系统
void Application::setupRenderSystem()
{
	if(!mRoot->restoreConfig())
	{
		if(!mRoot->showConfigDialog())
		{
			throw Exception(50,"无法设置渲染系统！","Application::setupRenderSystem()");
		}
	}
}

// 创建渲染窗口
void Application::createRenderWindow()
{
	mWindow = mRoot->initialise(true,"轩辕传奇");
}

// 初始化资源
void Application::initializeResourceGroups()
{
	ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();
}

void Application::createSceneManager()
{
	mSceneMgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE,"defaultSceneManager");
	HeightFunction::initialize(mSceneMgr);
	
	SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode",Vector3(20,0,20));

	/*	setup the camera */
	mCamera = mSceneMgr->createCamera("PlayerCamera");
	mCamera->setPosition(Vector3(0.0,GlobalVariables::CameraZoomBegin,GlobalVariables::CameraZoomBegin));
	mCamera->lookAt(Vector3(0.0,0.0,0.0));
	mCamera->setNearClipDistance(GV::Meter(0.5));
	//mCamera->setFarClipDistance(2000.0);
	mCamera->setAutoAspectRatio(true);
	Viewport *vp = mRoot->getAutoCreatedWindow()->addViewport(mCamera);

	SceneNode *camNode = node->createChildSceneNode("PlayerCameraNode",Vector3(0,GlobalVariables::TheOneHeight/2.0,0));
	camNode->attachObject(mCamera);
	mRoot->getAutoCreatedWindow()->addViewport(mCamera,10);

	mScene = new OgreMaxScene();
}

void Application::setupCollision()
{
	new CollisionManager(mSceneMgr);
	CollisionManager *cmgr = CollisionManager::getSingletonPtr();
	
	cmgr->addCollClass("player");
	cmgr->addCollType("player","player",COLLTYPE_QUICK);

	mCollContext = CollisionManager::getSingletonPtr()->getDefaultContext();
}
void Application::createTheOne()
{
	//	THE ONE
	SceneNode *node = mSceneMgr->getSceneNode("PlayerNode")->createChildSceneNode("TheOneNode");
	Entity *ent = mSceneMgr->createEntity("TheOne","DonghuaMain.mesh");
	node->attachObject(ent);
	node->scale(Vector3::UNIT_SCALE*GV::UnitsPerMeter/100.0);

	OgreOpcode::EntityCollisionShape *cshp = CollisionManager::getSingletonPtr()->createEntityCollisionShape(ent->getName());
	cshp->load(ent);

	mTheOneObject = mCollContext->createObject(ent->getName());
	mTheOneObject->setCollClass("player");
	mTheOneObject->setShape(cshp);
	mCollContext->addObject(mTheOneObject);
	mCollContext->reset();

	tObjectProperties *prop = new tObjectProperties(80,100,120,15,0);
	prop->mMovingSpeed = GV::Meter(1.0);

	GObject::G_TheOne->mSceneNode = node;
	GObject::G_TheOne->mEntity = ent;
	GObject::G_TheOne->addProperties(prop);

	GObject::G_TheOne->addAnimation(AS_IDLE);
	GObject::G_TheOne->addAnimation(AS_IDLE1);
	GObject::G_TheOne->addAnimation(AS_IDLE2);

	GObject::G_TheOne->addAnimation(AS_WALK,"WalkOnStone.mp3");
	GObject::G_TheOne->addAnimation(AS_RUN,"RunOnStone.mp3");
	
	GObject::G_TheOne->addAnimation(AS_JUMP_UP,"JumpUp.wav");
	GObject::G_TheOne->addAnimation(AS_JUMP_DOWN);
	GObject::G_TheOne->addAnimation(AS_JUMP_LAND);

	GObject::G_TheOne->addAnimation(AS_ATTACK,"Attack01.wav");
	GObject::G_TheOne->addAnimation(AS_ATTACK1,"Attack02.wav");
	GObject::G_TheOne->addAnimation(AS_ATTACK2,"Attack03.wav");

	GObject::G_TheOne->addAnimation(AS_SPELL,"Spell.wav");
	GObject::G_TheOne->playAnimation(AS_IDLE,false);
	AnimationState *animst = GObject::G_TheOne->mCurrentAnimation->mAnimState;
	animst->setTimePosition(animst->getLength()*0.5);

	GlobalVariables::TheOneJump_UT = GObject::G_TheOne->mAnimation.at(AS_JUMP_UP)->mAnimState->getLength();
	GlobalVariables::TheOneJump_DT = GObject::G_TheOne->mAnimation.at(AS_JUMP_DOWN)->mAnimState->getLength();
	GlobalVariables::TheOneJump_LT = GObject::G_TheOne->mAnimation.at(AS_JUMP_LAND)->mAnimState->getLength();

	// 鼠标点击特效
	ent = mSceneMgr->createEntity("mouseDownEffectEntity","mouseDownEffect.mesh");
	ent->setCastShadows(false);
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode("mouseDownEffectNode");
	node->attachObject(ent);
	node->scale(0.1,0.1,0.1);
	node->setVisible(false);
	animst = ent->getAnimationState("mouseDownEffect");
	animst->setEnabled(true);
	animst->setLoop(false);
}

// 设置OIS
void Application::setupInputSystem()
{
	size_t	winHnd = 0;
	std::ostringstream winHndStr;
	OIS::ParamList pl;
	RenderWindow *win = mRoot->getAutoCreatedWindow();

	win->getCustomAttribute("WINDOW",&winHnd);
	winHndStr<<winHnd;
	pl.insert(std::make_pair(std::string("WINDOW"),winHndStr.str()));
	mInputManager = OIS::InputManager::createInputSystem(pl);

	try
	{
		mKeyboard	= static_cast<OIS::Keyboard *>(mInputManager->createInputObject(OIS::OISKeyboard, true));
		mMouse		= static_cast<OIS::Mouse *>(mInputManager->createInputObject(OIS::OISMouse, true));
	}
	catch(OIS::Exception &e)
	{
		throw Exception(40,e.eText,"Application::setupInputSystem");
	}
	windowResized(win);
}

// 设置CEGUI
void Application::setupCEGUI()
{
	mCEGUIRenderer		= new CEGUI::OgreCEGUIRenderer(mRoot->getAutoCreatedWindow(),RENDER_QUEUE_OVERLAY,false,3000,mSceneMgr);
	mCEGUISystem		= new CEGUI::System(mCEGUIRenderer);
	CEGUI::Logger::getSingletonPtr()->setLoggingLevel(CEGUI::Informative);

	CEGUI::SchemeManager::getSingletonPtr()->loadScheme("TaharezLookSkin.scheme");
	//mCEGUISheet = CEGUI::WindowManager::getSingletonPtr()->loadWindowLayout("EnterMenu.layout");
	//mCEGUISystem->setGUISheet(mCEGUISheet);
	CEGUI::Font *font = CEGUI::FontManager::getSingletonPtr()->createFont("huawenfs-12.font");
	mCEGUISystem->setDefaultFont(font);
	mCEGUISystem->setDefaultMouseCursor("TaharezLook","MouseArrow");
	//CEGUI::MouseCursor::getSingletonPtr()->setImage("TaharezLook","MouseArrow");
	//CEGUI::MouseCursor::getSingletonPtr()->show();
}

//	FMOD soundmanager
void Application::setupSoundManager()
{
	mSoundMgr = SoundManager::createInstance(mCamera);
	mBackSound = mSoundMgr->createSound("BackGround.mp3",true,false,0,FMOD_CREATESTREAM);
	mSoundMgr->playSound(mBackSound);
	mBackSound->mChannel->setVolume(GlobalVariables::BackSoundVolume);
}

//	create ogremax scene
void Application::createScene()
{
	//mSceneMgr->setWorldGeometry("terrain.cfg");
	//mSceneMgr->setSkyDome(true,"Examples/CloudySky",8,3);

	/* the max scene */
	mScene->initPG(mCamera,GV::Meter(140.0));

	/*	setup tree rendering PG */
	mScene->initPGTrees(GV::Meter(10.0),false);
	mScene->setPGTree(Vector2(GV::Meter(50.0),GV::Meter(5.0)),
		Vector2(GV::Meter(55.0),GV::Meter(1.0)));

	/*	setup bush rendering PG */
	mScene->initPGBushes(GV::Meter(10.0),false);
	mScene->setPGBush(Vector2(GV::Meter(20.0),GV::Meter(5.0)),
		Vector2(GV::Meter(25.0),GV::Meter(1.0)));

	///*	setup grass in PG */
	//mScene->initPGGrass(GV::Meter(3.0),false);
	//mScene->setPGGrass(GV::Meter(20.0));

	/*	set height function for trees bushes and grass */
	mScene->setPGHeightFunction(PAGE_MODE(PM_TREE|PM_BUSH), &HeightFunction::getTerrainHeight);

	///*	add some grass here */
	//mScene->addPGGrass(String("grass"),
	//	Vector2(GV::Meter(0.2),GV::Meter(0.3)),
	//	Vector2(GV::Meter(0.4),GV::Meter(0.5)),
	//	0.1,false);

	//Entity *tree = mSceneMgr->createEntity("tree","fir05_30.mesh");
	//Entity *bush = mSceneMgr->createEntity("bush","shroom1_3.mesh");

	//for(int i=0;i<15;i++)
	//	//for(int j=0;i<15;j++)
	//	{
	//		mScene->addPGTree(tree,Vector3(i*GV::Meter(10.0),0,i*GV::Meter(10.0)));
	//		mScene->addPGBush(bush,Vector3(i*GV::Meter(10.0),0,i*GV::Meter(10.0)));
	//	}

	SceneNode *node = 0;

	node = mSceneMgr->getRootSceneNode()->createChildSceneNode("max scene");
	mScene->Load("textscene.scene",mWindow,OgreMaxScene::NO_OPTIONS,mSceneMgr,node);
	//node = mSceneMgr->getSceneNode("Terrain");
	//node->setPosition(Vector3::ZERO);
	
	Real xcam_pos = GV::Meter(50.0);
	Real zcam_pos = GV::Meter(50.0);
	Real height = HeightFunction::getTerrainHeight(xcam_pos,zcam_pos) + GlobalVariables::TheOneHeight/2.0;
	mSceneMgr->getSceneNode("PlayerNode")->setPosition(xcam_pos,height,zcam_pos);

}
//	create OgreOpcode objects
void Application::createCollision()
{
	CollisionManager *cmgr = CollisionManager::getSingletonPtr();
	OgreOpcode::EntityCollisionShape *cshp = 0;
	OgreOpcode::CollisionObject *cobj = 0;

	SceneManager::MovableObjectIterator itr = mSceneMgr->getMovableObjectIterator("Entity");
	while(itr.hasMoreElements())
	{
		Entity *ent = static_cast<Entity *>(itr.peekNextValue());
		//SceneNode *node = 0;
		//node = ent->getParentSceneNode();
		String str = ent->getName().substr(0,2);
		if(str == "EX")
		{
			/*	setup exact collision object */
			cshp = cmgr->createEntityCollisionShape(ent->getName());
			cshp->load(ent);
			cobj = mCollContext->createObject(ent->getName());
			cobj->setCollClass("player");
			cobj->setShape(cshp);
			mCollContext->addObject(cobj);
			ent->setVisible(false);
		}
		else if(str == "QK")
		{
			/*	setup quick collision object */
			cshp = cmgr->createEntityCollisionShape(ent->getName());
			cshp->load(ent);
			cobj = mCollContext->createObject(ent->getName());
			cobj->setCollClass("player");
			cobj->setShape(cshp);
			mCollContext->addObject(cobj);
			ent->setVisible(false);
		}
		itr.moveNext();
	}
}
// 建立帧监听器
void Application::createFrameListener()
{
	mListener = new ApplicationListener(mSceneMgr,mKeyboard,mMouse,mScene,mTheOneObject);
	mListener->showDebugOverlay(true);
	mRoot->addFrameListener(mListener);
}

// 启动渲染循环
void Application::startRenderLoop()
{
	mRoot->startRendering();
}