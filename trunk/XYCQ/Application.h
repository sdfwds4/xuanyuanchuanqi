#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "GlobalDefines.h"
#include "ApplicationListener.h"
#include "SoundManager.h"
#include "GObject.h"

class Application : public WindowEventListener
{
public:	
	Application();
	~Application();	
	void go();
private:
	void createRoot();
	void defineResources();
	void setupRenderSystem();
	void createRenderWindow();
	void initializeResourceGroups();
	void createSceneManager();

	void setupInputSystem();
	void setupCEGUI();
	void setupSoundManager();
	void setupCollision();

	void createTheOne();
	void createFrameListener();

	void createScene();
	void createCollision();
	void startRenderLoop();

	/*	window event */
	void windowResized(RenderWindow *rw);
	void windowClosed(RenderWindow *rw);

	Root *mRoot;
	OIS::Keyboard *mKeyboard;
	OIS::Mouse *mMouse;
	OIS::InputManager *mInputManager;
	ApplicationListener *mListener;
	SoundManager *mSoundMgr;

	RenderWindow *mWindow;
	OgreMax::OgreMaxScene *mScene;
	SceneManager *mSceneMgr;
	OgreOpcode::CollisionContext *mCollContext;
	OgreOpcode::CollisionObject *mTheOneObject;

	ParticleSystem *mParticle;

	tNodeSound *mBackSound;
	CEGUI::Renderer *mCEGUIRenderer;
	CEGUI::System *mCEGUISystem;
	CEGUI::Window *mCEGUISheet;
	Camera *mCamera;
};

#endif