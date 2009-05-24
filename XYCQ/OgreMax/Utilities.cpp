/*
 * OgreMaxViewer - An Ogre 3D-based viewer for .scene and .mesh files
 * Copyright 2008 Derek Nedelman
 *
 * This code is available under the OgreMax Free License:
 *   -You may use this code for any purpose, commercial or non-commercial.
 *   -If distributing derived works (that use this source code) in binary or source code form, 
 *    you must give the following credit in your work's end-user documentation: 
 *        "Portions of this work provided by OgreMax (www.ogremax.com)"
 *
 * Derek Nedelman assumes no responsibility for any harm caused by using this code.
 * 
 * OgreMaxViewer was written by Derek Nedelman and released at www.ogremax.com 
 */


//Includes---------------------------------------------------------------------
#include "Utilities.hpp"

using namespace Ogre;
using namespace OgreMax;


//Implementation---------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <CoreFoundation/CoreFoundation.h>

// This function will locate the path to our application on OS X,
// unlike windows you can not rely on the curent working directory
// for locating your configuration files and resources.
String Utilities::GetMacBundlePath()
{
    char path[1024];
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    assert(mainBundle);

    CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
    assert(mainBundleURL);

    CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
    assert(cfStringRef);

    CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

    CFRelease(mainBundleURL);
    CFRelease(cfStringRef);

    return String(path);
}

#endif

String Utilities::GetApplicationResourcesPath()
{    
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    return GetMacBundlePath() + "/Contents/Resources/";
#else
    return StringUtil::BLANK;
#endif
}

bool Utilities::EndsWithNoCase(const String& text, const String& endsWith)
{
    bool result = false;

    size_t textLength = text.length();
    size_t endsWithLength = endsWith.length();
    if (endsWithLength <= textLength)
    {
        //Get the end string, lowercase
        String end = text.substr(textLength - endsWithLength);
        StringUtil::toLowerCase(end);
        
        //Get the 'ends with' string, lowercase
        String endsWithLower = endsWith;
        StringUtil::toLowerCase(endsWithLower);

        result = end == endsWithLower;
    }

    return result;
}

bool Utilities::ImageCodecCanCode(const Ogre::String& name)
{
    if (name == "dds" || 
        name == "cut" || 
        name == "g3" || 
        name == "gif" || 
        name == "hdr" || 
        name == "ico" || 
        name == "iff" || 
        name == "jng" || 
        name == "koa" || 
        name == "lbm" || 
        name == "mng" || 
        name == "pcd" || 
        name == "pcx" || 
        name == "psd" || 
        name == "ras" || 
        name == "sgi" || 
        name == "wap" || 
        name == "wbm" || 
        name == "wbmp" || 
        name == "xbm")
    {
        return false;
    }
    else
        return true;
}