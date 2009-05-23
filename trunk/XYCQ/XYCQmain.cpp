//#include "globalHeaders.h"
#include "Application.h"
#include "ApplicationListener.h"

#pragma comment(lib,"OgreMain_d.lib")
#pragma comment(lib,"OIS_d.lib")
#pragma comment(lib,"CEGUIBase_d.lib")
#pragma comment(lib,"OgreGUIRenderer_d.lib")

#pragma comment(lib,"fmodex_vc.lib")

#pragma comment(lib,"OgreOpcode_d.lib")

#pragma comment(lib,"OgreMax.lib")
#pragma comment(lib,"PagedGeometry.lib")
//#pragma comment(lib,"newton.lib")
//#pragma comment(lib,"OgreNewt_Main_d.lib")

using namespace Ogre;
using namespace OgreOpcode;

#if		OGRE_PLATFORM == OGRE_PLATFORM_WIN32
// Win32 程序
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
INT WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,INT nShowCmd)

#else
// 一般程序
int main(int argc,char **argv)
#endif
{
	try
	{
		Application app;
		app.go();
	}
	catch(Exception& e)
	{
#if		OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox(NULL,e.getFullDescription().c_str(),"捕获异常",MB_OK);
#else
		sprintf(stderr,"捕获异常：%s",e.getFullDescription().c_str());
#endif
	}
	return 0;
}