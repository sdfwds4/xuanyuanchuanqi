#include "Application.h"
#include "HeightFunction.h"

//	全局变量设置：
const float GlobalVariables::CameraZoomStep					= 50.0;
const float GlobalVariables::CameraZoomBegin				= 300.0;
const float GlobalVariables::CameraZoomMin					= 200.0;
const float GlobalVariables::CameraZoomMax					= 400.0;

float GlobalVariables::CameraPitchMin						= 45.0;
float GlobalVariables::CameraPitchMax						= 90.0;
float GlobalVariables::CameraPitchCurrent					= 60.0;

bool GlobalVariables::PlayerCameraLocked					= false;
bool GlobalVariables::PlayerPositionLocked					= false;

float GlobalVariables::TheOneScale							= 1.0;
float GlobalVariables::TheOneHeight							= 150;
AnimationStatus GlobalVariables::TheOneMoveAS				= AS_WALK;
AnimationStatus GlobalVariables::TheOneIdleAS				= AS_IDLE;
AnimationStatus GlobalVariables::TheOneAttackAS				= AS_ATTACK;

bool GlobalVariables::TheOneJumping							= false;
float GlobalVariables::TheOneJump_UT						= 0.1667;
float GlobalVariables::TheOneJump_DT						= 0.5;
float GlobalVariables::TheOneJump_LT						= 0.3333;

float GlobalVariables::TheOneJump_H							= 0.0;
float GlobalVariables::TheOneJump_h							= 0.0;
float GlobalVariables::TheOneJump_V							= 0.0;
float GlobalVariables::TheOneJump_T							= 0.0;
float GlobalVariables::TheOneJump_t							= 0.0;

const float GlobalVariables::Gravity						= 980;
float GlobalVariables::BackSoundVolume						= 0.2;

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
	createCollision();
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

	//	摄像机
	mCamera = mSceneMgr->createCamera("PlayerCamera");
	mCamera->setPosition(Vector3(0.0,GlobalVariables::CameraZoomBegin,GlobalVariables::CameraZoomBegin));
	mCamera->lookAt(Vector3(0.0,0.0,0.0));
	mCamera->setNearClipDistance(20.0);
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

	OgreOpcode::EntityCollisionShape *cshp = CollisionManager::getSingletonPtr()->createEntityCollisionShape(ent->getName());
	cshp->load(ent);

	mTheOneObject = mCollContext->createObject(ent->getName());
	mTheOneObject->setCollClass("player");
	mTheOneObject->setShape(cshp);
	mCollContext->addObject(mTheOneObject);
	mCollContext->reset();

	tObjectProperties *prop = new tObjectProperties(80,100,120,15,0);
	prop->mMovingSpeed = 200;

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
	node->scale(0.1*GlobalVariables::TheOneScale,0.1*GlobalVariables::TheOneScale,0.1*GlobalVariables::TheOneScale);
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
void Application::setupSoundManager()
{
	//	声音管理器
	mSoundMgr = SoundManager::createInstance(mCamera);
	mBackSound = mSoundMgr->createSound("BackGround.mp3",true,false,0,FMOD_CREATESTREAM);
	mSoundMgr->playSound(mBackSound);
	mBackSound->mChannel->setVolume(GlobalVariables::BackSoundVolume);
}

//	create ogremax scene
void Application::createScene()
{
	mScene->initPG(mCamera,200000);

	mScene->initPGTrees(200,false);
	mScene->setPGTree(Vector2(4000,100),Vector2(4500,100));

	mScene->initPGBushes(200,false);
	mScene->setPGBush(Vector2(2000,100),Vector2(2500,100));

	mScene->initPGGrass(100,false);
	mScene->setPGGrass(500);

	mScene->setPGHeightFunction((PM_TREE), &HeightFunction::getTerrainHeight);

	mScene->addPGGrass(String("grass"),Vector2(50.0,60.0),Vector2(70.0,80.0),0.002,false);
	mScene->Load("LuoHuaVillage.scene",mWindow,OgreMaxScene::NO_OPTIONS,mSceneMgr);
	
	Real xcam_pos = 4000;
	Real zcam_pos = 4000;
	Real height = mListener->getTerrainHeight(xcam_pos,zcam_pos) + GlobalVariables::TheOneScale*GlobalVariables::TheOneHeight/2.0;
	mSceneMgr->getSceneNode("PlayerNode")->setPosition(xcam_pos,height,zcam_pos);

}
//	建立OgreOpcode实体
void Application::createCollision()
{
	CollisionManager *cmgr = CollisionManager::getSingletonPtr();
	OgreOpcode::EntityCollisionShape *cshp = 0;
	OgreOpcode::CollisionObject *cobj = 0;

	SceneManager::MovableObjectIterator itr = mSceneMgr->getMovableObjectIterator("Entity");
	while(itr.hasMoreElements())
	{
		Entity *ent = static_cast<Entity *>(itr.peekNextValue());
		String str = ent->getName().substr(0,5);
		if(str=="House")
		{
			//ent->getParentSceneNode()->showBoundingBox(true);
			cshp = cmgr->createEntityCollisionShape(ent->getName());
			cshp->load(ent);
			cobj = mCollContext->createObject(ent->getName());
			cobj->setCollClass("player");
			cobj->setShape(cshp);
			mCollContext->addObject(cobj);
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